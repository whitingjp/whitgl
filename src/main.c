#include <stdbool.h>

#include <whitgl/input.h>
#include <whitgl/logging.h>
#include <whitgl/math.h>
#include <whitgl/sound.h>
#include <whitgl/sys.h>

void draw(whitgl_ivec size)
{
	whitgl_iaabb full = {{0,0},{16,16}};
	whitgl_iaabb recta = {{size.x-4-32, size.y-4-32}, {size.x-2, size.y-4}};
	whitgl_sys_draw_tex_iaabb(0, full, recta);
	whitgl_iaabb doub = {{-16,-16},{16,16}};
	whitgl_iaabb rectb = {{50, 50}, {66, 66}};
	whitgl_sys_draw_tex_iaabb(1, doub, rectb);
	whitgl_iaabb rectc = {{10, 10}, {25, 25}};
	whitgl_sys_color col = {255, 10, 120, 255};
	whitgl_sys_draw_iaabb(rectc, col);
}

int main()
{
	WHITGL_LOG("Starting main.");

	whitgl_sys_setup setup = whitgl_default_setup;
	setup.name = "main";

	if(!whitgl_sys_init(setup))
		return 1;
	whitgl_sound_init();
	whitgl_input_init();

	whitgl_sound_add(0, "data/beam.ogg");
	whitgl_sound_play(0, 1);

	whitgl_sys_add_image(0, "data/whit.png");
	whitgl_sys_add_image(1, "data/whittwo.png");

	bool running = true;
	while(running)
	{
		whitgl_sound_update();
		whitgl_input_update();

		whitgl_sys_draw_init();
		draw(setup.size);
		// whitgl_ivec mousepos = whitgl_input_mouse_pos(setup.pixel_size);
		// WHITGL_LOG("mousex: %d mousey: %d", mousepos.x, mousepos.y);
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
