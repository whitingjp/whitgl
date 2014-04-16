
typedef struct
{
	const char* name;
	int width;
	int height;
	int pixel_size;
	bool fullscreen;
	bool disable_mouse_cursor;
} whitgl_sys_setup;

extern whitgl_sys_setup whitgl_default_setup;

void whitgl_sys_init(whitgl_sys_setup setup);
bool whitgl_sys_should_close();
void whitgl_sys_close();

void whitgl_sys_draw_init();
void whitgl_sys_draw_finish();

double whitgl_sys_get_time();
void whitgl_sys_sleep( double time );
