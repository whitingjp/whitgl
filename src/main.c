#include <stdbool.h>

#include <whitgl/input.h>
#include <whitgl/logging.h>
#include <whitgl/sound.h>
#include <whitgl/sys.h>

int main()
{
	WHITGL_LOG("Starting main.");

	whitgl_sys_setup setup = whitgl_default_setup;
	setup.name = "main";
	
	whitgl_sys_init(setup);
	whitgl_sound_init();
	whitgl_input_init();

	whitgl_sound_add(0, "data/beam.ogg");
	whitgl_sound_play(0, 1);

	bool running = true;
	while(running)
	{
		whitgl_sound_update();
		whitgl_input_update();

		whitgl_sys_draw_init();
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
