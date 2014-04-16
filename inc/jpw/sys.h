
typedef struct
{
	const char* name;
	int width;
	int height;
	int pixel_size;
	bool fullscreen;
	bool disable_mouse_cursor;
} jpw_sys_setup;

extern jpw_sys_setup jpw_default_setup;

void jpw_sys_init(jpw_sys_setup setup);
bool jpw_sys_should_close();
void jpw_sys_close();

void jpw_sys_draw_init();
void jpw_sys_draw_finish();

double jpw_sys_get_time();
void jpw_sys_sleep( double time );
