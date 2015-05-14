#include <stdbool.h>
#include <stddef.h>

#include <whitgl/input.h>
#include <whitgl/logging.h>
#include <whitgl/math.h>
#include <whitgl/sound.h>
#include <whitgl/sys.h>
#include <whitgl/timer.h>

const char* post_src = "\
#version 150\
\n\
in vec2 Texturepos;\
out vec4 outColor;\
uniform sampler2D tex;\
uniform float spread;\
void main()\
{\
	vec2 offset = vec2(0.1*spread);\
	outColor = vec4(texture( tex, Texturepos-offset ).r, texture( tex, Texturepos ).g, texture( tex, Texturepos+offset ).ba);\
}\
";

void draw(whitgl_ivec size)
{
	(void)size;
	whitgl_iaabb screen = {{0, 0}, {size.x, size.y}};
	whitgl_sys_color col = {0x35, 0x52, 0x76, 0xff};
	whitgl_sys_draw_iaabb(screen, col);
	whitgl_sprite sprite = {0, {0,0},{16,16}};
	whitgl_ivec frametr = {1, 0};
	whitgl_ivec pos = {16,0};
	whitgl_sys_draw_sprite(sprite, frametr, pos);
	whitgl_ivec framebr = {1, 1};
	pos.x = 0; pos.y = 16;
	whitgl_sys_draw_sprite(sprite, framebr, pos);
}

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
	if(!whitgl_change_shader(WHITGL_SHADER_POST, post_shader))
		return 1;

	whitgl_sound_init();
	whitgl_input_init();

	whitgl_sound_add(0, "data/beam.ogg");
	whitgl_sound_play(0, 1);

	whitgl_sys_add_image(0, "data/sprites.png");

	whitgl_timer_init();
	whitgl_float elapsed_time = 0;

	bool running = true;
	while(running)
	{
		whitgl_sound_update();

		elapsed_time += whitgl_timer_next_frame();
		while(elapsed_time > 0)
		{
			whitgl_input_update();
			elapsed_time -= 1.0/60.0;
		}

		whitgl_sys_draw_init();
		draw(setup.size);
		whitgl_ivec mousepos = whitgl_input_mouse_pos(setup.pixel_size);
		whitgl_set_shader_uniform(WHITGL_SHADER_POST, 0, ((float)mousepos.x-setup.size.x/2)/30);
		whitgl_sys_draw_finish();

		if(whitgl_input_pressed(WHITGL_INPUT_ESC))
			running = false;
		if(whitgl_sys_should_close())
			running = false;
	}

	whitgl_input_shutdown();
	whitgl_sound_shutdown();

	whitgl_sys_close();

	return 0;
}
