#include <stdbool.h>
#include <stddef.h>

#include <whitgl/input.h>
#include <whitgl/logging.h>
#include <whitgl/math.h>
#include <whitgl/random.h>
#include <whitgl/sound.h>
#include <whitgl/sys.h>
#include <whitgl/timer.h>

const char* post_src = "\
#version 150\
\n\
in vec2 Texturepos;\
out vec4 outColor;\
uniform sampler2D tex;\
uniform sampler2D extra;\
uniform float spread;\
void main()\
{\
	vec2 offset = vec2(0.1*spread);\
	outColor = vec4(texture( tex, Texturepos-offset ).r, texture( tex, Texturepos ).g, texture( tex, Texturepos+offset ).ba);\
	outColor = outColor + texture(extra, Texturepos);\
}\
";

int main()
{
	WHITGL_LOG("Starting main.");

	whitgl_sys_setup setup = whitgl_sys_setup_zero;
	setup.size.x = 32;
	setup.size.y = 32;
	setup.pixel_size = 16;
	setup.name = "main";

	if(!whitgl_sys_init(&setup))
		return 1;
	whitgl_shader post_shader = whitgl_shader_zero;
	post_shader.fragment_src = post_src;
	post_shader.num_uniforms = 1;
	post_shader.uniforms[0] = "spread";
	post_shader.num_images = 1;
	post_shader.images[0] = "extra";
	if(!whitgl_change_shader(WHITGL_SHADER_POST, post_shader))
		return 1;

	whitgl_sound_init();
	whitgl_input_init();

	whitgl_sound_add(0, "data/beam.ogg");
	whitgl_sound_play(0, 1);

	whitgl_sys_add_image(0, "data/sprites.png");
	whitgl_random_seed seed = whitgl_random_seed_init(0);
	unsigned char data_texture[setup.size.x*setup.size.y*4];
	whitgl_int i;
	for(i=0; i<setup.size.x*setup.size.y*4; i+=4)
	{
		whitgl_int pixel = i/4;
		data_texture[i] = (pixel%32)*8;
		data_texture[i+1] = (pixel/32)*8;
		data_texture[i+2] = whitgl_random_int(&seed, 32)+128;
		data_texture[i+3] = 255;
	}
	whitgl_sys_add_image_from_data(1, setup.size, data_texture);

	whitgl_timer_init();
	whitgl_float uniform = 0;

	bool running = true;
	while(running)
	{
		whitgl_sound_update();

		whitgl_timer_tick();
		while(whitgl_timer_should_do_frame(60))
		{
			whitgl_input_update();
			whitgl_ivec mousepos = whitgl_input_mouse_pos(setup.pixel_size);
			uniform = ((float)mousepos.x-setup.size.x/2)/30;

			if(whitgl_input_pressed(WHITGL_INPUT_ESC))
				running = false;
			if(whitgl_sys_should_close())
				running = false;
		}

		for(i=0; i<setup.size.x*setup.size.y*4; i+=4)
			data_texture[i+2] = whitgl_random_int(&seed, 32)+128;

		whitgl_sys_update_image_from_data(1, setup.size, data_texture);

		whitgl_sys_draw_init();

		whitgl_sprite sprite = {0, {0,0},{16,16}};
		whitgl_ivec frametr = {1, 0};
		whitgl_ivec pos = {16,0};
		whitgl_sys_draw_sprite(sprite, frametr, pos);
		whitgl_ivec framebr = {1, 1};
		pos.x = 0; pos.y = 16;
		whitgl_sys_draw_sprite(sprite, framebr, pos);

		whitgl_set_shader_uniform(WHITGL_SHADER_POST, 0, uniform);
		whitgl_set_shader_image(WHITGL_SHADER_POST, 0, 1);
		whitgl_sys_draw_finish();
	}

	whitgl_input_shutdown();
	whitgl_sound_shutdown();

	whitgl_sys_close();

	return 0;
}
