#include <stdbool.h>

#include <whitgl/input.h>
#include <whitgl/logging.h>
#include <whitgl/math.h>
#include <whitgl/sound.h>
#include <whitgl/sys.h>

void draw(whitgl_ivec size)
{
	whitgl_iaabb screen = {{0, 0}, {size.x, size.y}};
	whitgl_sys_color col = {0x35, 0x52, 0x76, 0xff};
	whitgl_sys_draw_iaabb(screen, col);
	whitgl_sprite sprite = {0, {0,0},{16,16}};
	whitgl_ivec frametr = {1, 0};
	whitgl_ivec pos = {30,10};
	whitgl_sys_draw_sprite(sprite, frametr, pos);
	whitgl_ivec framebr = {1, 1};
	pos.x = 50; pos.y = 30;
	whitgl_sys_draw_sprite(sprite, framebr, pos);
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

	whitgl_sys_add_image(0, "data/sprites.png");

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
