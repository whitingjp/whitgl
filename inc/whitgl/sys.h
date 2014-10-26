#ifndef WHITGL_SYS_H_
#define WHITGL_SYS_H_

#include <stdbool.h>
#include <stddef.h>

#include <whitgl/math.h>

typedef struct
{
	const char* name;
	whitgl_ivec size;
	int pixel_size;
	bool fullscreen;
	bool disable_mouse_cursor;
	bool vsync;
} whitgl_sys_setup;
extern whitgl_sys_setup whitgl_default_setup;

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

typedef enum
{
	WHITGL_SHADER_FLAT,
	WHITGL_SHADER_TEXTURE,
	WHITGL_SHADER_POST,
	WHITGL_SHADER_MAX,
} whitgl_shader_slot;

#define WHITGL_MAX_SHADER_UNIFORMS (4)
#define WHITGL_MAX_SHADER_COLORS (4)
typedef struct
{
	const char* vertex_src;
	const char* fragment_src;
	int num_uniforms;
	const char* uniforms[WHITGL_MAX_SHADER_UNIFORMS];
	int num_colors;
	const char* colors[WHITGL_MAX_SHADER_COLORS];
} whitgl_shader;
static const whitgl_shader whitgl_shader_zero = {NULL, NULL, 0, {}, 0, {}};

bool whitgl_sys_init(whitgl_sys_setup* setup);
bool whitgl_sys_should_close();
void whitgl_sys_close();

void whitgl_sys_set_clear_color(whitgl_sys_color col);

bool whitgl_change_shader(whitgl_shader_slot type, whitgl_shader shader);
void whitgl_set_shader_uniform(whitgl_shader_slot type, int uniform, float value);
void whitgl_set_shader_color(whitgl_shader_slot type, int color, whitgl_sys_color value);

void whitgl_sys_draw_init();
void whitgl_sys_draw_finish();

void whitgl_sys_add_image(int id, const char* filename);
void whitgl_sys_image_from_data(int id, whitgl_ivec size, const unsigned char* data);
void whitgl_sys_draw_iaabb(whitgl_iaabb rectangle, whitgl_sys_color col);
void whitgl_sys_draw_hollow_iaabb(whitgl_iaabb rect, whitgl_int width, whitgl_sys_color col);
void whitgl_sys_draw_fcircle(whitgl_fcircle circle, whitgl_sys_color col, int tris);
void whitgl_sys_draw_tex_iaabb(int id, whitgl_iaabb src, whitgl_iaabb dest);
void whitgl_sys_draw_sprite(whitgl_sprite sprite, whitgl_ivec frame, whitgl_ivec pos);

double whitgl_sys_get_time();

#endif // WHITGL_SYS_H_