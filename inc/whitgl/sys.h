#ifndef WHITGL_SYS_H_
#define WHITGL_SYS_H_

#include <whitgl/math.h>

typedef struct
{
	const char* name;
	whitgl_ivec size;
	int pixel_size;
	bool fullscreen;
	bool disable_mouse_cursor;
} whitgl_sys_setup;

typedef struct
{
	unsigned char r,g,b,a;
} whitgl_sys_color;
static const whitgl_sys_color whitgl_sys_color_zero = {0,0,0,0};

typedef struct
{
	int image;
	whitgl_ivec top_left;
	whitgl_ivec size;
} whitgl_sprite;
static const whitgl_sprite whitgl_sprite_zero = {0,	{0,0}, {0,0}};

extern whitgl_sys_setup whitgl_default_setup;

bool whitgl_sys_init(whitgl_sys_setup setup);
bool whitgl_sys_should_close();
void whitgl_sys_close();

void whitgl_sys_draw_init();
void whitgl_sys_draw_finish();

void whitgl_sys_add_image(int id, const char* filename);
void whitgl_sys_draw_iaabb(whitgl_iaabb rectangle, whitgl_sys_color col);
void whitgl_sys_draw_tex_iaabb(int id, whitgl_iaabb src, whitgl_iaabb dest);
void whitgl_sys_draw_sprite(whitgl_sprite sprite, whitgl_ivec frame, whitgl_ivec pos);

double whitgl_sys_get_time();
void whitgl_sys_sleep( double time );

#endif // WHITGL_SYS_H_