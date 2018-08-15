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
	setup.resizable = true;

	if(!whitgl_sys_init(&setup))
		return 1;
	whitgl_shader post_shader = whitgl_shader_zero;
	post_shader.fragment_src = post_src;
	post_shader.num_uniforms = 2;
	post_shader.uniforms[0].name = "spread";
	post_shader.uniforms[0].type = WHITGL_UNIFORM_FLOAT;
	post_shader.uniforms[1].name = "extra";
	post_shader.uniforms[1].type = WHITGL_UNIFORM_IMAGE;

	// whitgl_sys_color bg = {0xc7,0xb2,0xf6,0xff};
	// whitgl_sys_set_clear_color(bg);
	if(!whitgl_change_shader(WHITGL_SHADER_POST, post_shader))
	 	return 1;

	whitgl_sound_init();
	whitgl_input_init();

	whitgl_loop_set_listener(whitgl_fvec_zero, whitgl_fvec_zero, 0);

	whitgl_sound_add(0, "data/beam.ogg");
	whitgl_sound_play(0, 1, 1);

	whitgl_loop_add_positional(1, "data/loop.ogg");
	whitgl_loop_set_paused(1, false);

	whitgl_sys_add_image(0, "data/sprites.png");
	whitgl_random_seed seed = whitgl_random_seed_init(0);
	whitgl_ivec texture_size = {32,32};
	unsigned char data_texture[texture_size.x*texture_size.y*4];
	whitgl_int i;
	for(i=0; i<texture_size.x*texture_size.y*4; i+=4)
	{
		whitgl_int pixel = i/4;
		data_texture[i] = (pixel%32)*8;
		data_texture[i+1] = (pixel/32)*8;
		data_texture[i+2] = whitgl_random_int(&seed, 32)+128;
		data_texture[i+3] = 255;
	}
	whitgl_sys_add_image_from_data(1, texture_size, data_texture);

	whitgl_load_model(0, "data/torus.wmd");
	whitgl_load_model(1, "data/cube.wmd");

	whitgl_timer_init();
	whitgl_float uniform = 0;

	whitgl_float time = 0;
	whitgl_int shape = 0;
	whitgl_fvec old_sound_pos = whitgl_fvec_zero;

	bool running = true;
	while(running)
	{
		whitgl_sound_update();

		whitgl_timer_tick();
		while(whitgl_timer_should_do_frame(60))
		{
			time += 1/60.0f;
			whitgl_input_update();
			whitgl_ivec mousepos = whitgl_input_mouse_pos(setup.pixel_size);
			uniform = ((float)mousepos.x-setup.size.x/2)/30;

			whitgl_fvec sound_pos = whitgl_fvec_scale_val(whitgl_fvec_sub(whitgl_fvec_divide(whitgl_ivec_to_fvec(mousepos), whitgl_ivec_to_fvec(setup.size)), whitgl_fvec_val(0.5)),20);
			whitgl_fvec sound_velocity = whitgl_fvec_sub(sound_pos, old_sound_pos);
			old_sound_pos = sound_pos;
			whitgl_loop_set_position(1, sound_pos, sound_velocity);
			if(whitgl_input_pressed(WHITGL_INPUT_A))
				shape = (shape+1)%2;
			if(whitgl_input_pressed(WHITGL_INPUT_ESC))
				running = false;
			if(whitgl_sys_should_close())
				running = false;
		}

		for(i=0; i<texture_size.x*texture_size.y*4; i+=4)
			data_texture[i+2] = whitgl_random_int(&seed, 32)+128;

		whitgl_sys_update_image_from_data(1, texture_size, data_texture);

		whitgl_sys_draw_init(0);

		whitgl_float fov = whitgl_pi/2;
		whitgl_fmat perspective = whitgl_fmat_perspective(fov, (float)setup.size.x/(float)setup.size.y, 0.1f, 10.0f);
		whitgl_fvec3 up = {0,1,0};
		whitgl_fvec3 camera_pos = {0,0,-2};
		whitgl_fvec3 camera_to = {0,0,0};
		whitgl_fmat view = whitgl_fmat_lookAt(camera_pos, camera_to, up);

		whitgl_fmat model_matrix = whitgl_fmat_rot_y(time);
		model_matrix = whitgl_fmat_multiply(model_matrix, whitgl_fmat_rot_z(time*3));

		whitgl_sys_draw_model(shape, WHITGL_SHADER_MODEL, model_matrix, view, perspective);

		whitgl_sprite sprite = {0, {0,0},{16,16}};
		whitgl_ivec frametr = {1, 0};
		whitgl_ivec pos = {16,0};
		whitgl_sys_draw_sprite(sprite, frametr, pos);
		whitgl_ivec framebr = {1, 1};
		pos.x = 0; pos.y = 16;
		whitgl_sys_draw_sprite(sprite, framebr, pos);

		whitgl_iaabb line = {{1,1},{15,15}};
		whitgl_sys_draw_line(line, whitgl_sys_color_white);

		whitgl_fcircle circle = {{24,24},6};
		whitgl_sys_draw_fcircle(circle, whitgl_sys_color_white, 8);

		whitgl_set_shader_float(WHITGL_SHADER_POST, 0, uniform);
		whitgl_set_shader_image(WHITGL_SHADER_POST, 1, 1);
		whitgl_sys_draw_finish();

		if(!whitgl_sys_window_focused())
			whitgl_timer_sleep(1.0/30.0);
	}

	whitgl_input_shutdown();
	whitgl_sound_shutdown();

	whitgl_sys_close();

	return 0;
}
