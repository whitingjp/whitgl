#include <stdbool.h>

#include <jpw/input.h>
#include <jpw/logging.h>
#include <jpw/sound.h>
#include <jpw/sys.h>

int main()
{
	JPW_LOG("Starting main.");

	jpw_sys_setup setup = jpw_default_setup;
	setup.name = "main";
	
	jpw_sys_init(setup);
	jpw_sound_init();
	jpw_input_init();

	jpw_sound_add(0, "data/beam.ogg");
	jpw_sound_play(0, 1);

	bool running = true;
	while(running)
	{
		jpw_sound_update();
		jpw_input_update();

		jpw_sys_draw_init();
		jpw_sys_draw_finish();

		if(jpw_input_pressed(JPW_INPUT_ESC))
			running = false;
		if(jpw_sys_should_close())
			running = false;
	}

	jpw_input_shutdown();
	jpw_sound_shutdown();

	jpw_sys_close();

	return 0;
}
