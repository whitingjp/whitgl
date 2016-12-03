#ifndef WHITGL_SYS_H_
#define WHITGL_SYS_H_

#include <stdbool.h>
#include <stddef.h>

#include <whitgl/math.h>
#include <whitgl/math3d.h>

typedef enum
{
	CURSOR_SHOW,
	CURSOR_HIDE,
	CURSOR_DISABLE,
} whitgl_sys_cursor_mode;

typedef struct
{
	const char* name;
	whitgl_ivec size;
	int pixel_size;
	whitgl_bool fullscreen;
	whitgl_sys_cursor_mode cursor;
	whitgl_bool vsync;
	whitgl_bool exact_size;
	whitgl_bool over_render;
	whitgl_bool start_focused;
	whitgl_bool clear_buffer;
} whitgl_sys_setup;
static const whitgl_sys_setup whitgl_sys_setup_zero =
{
	"default window name",
	{120, 80},
	4,
	false,
	CURSOR_SHOW,
	true,
	false,
	false,
	true,
	true,
};

typedef struct
{
	unsigned char r,g,b,a;
} whitgl_sys_color;
static const whitgl_sys_color whitgl_sys_color_zero = {0,0,0,0};
static const whitgl_sys_color whitgl_sys_color_white = {0xff,0xff,0xff,0xff};
static const whitgl_sys_color whitgl_sys_color_black = {0x00,0x00,0x00,0xff};


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
	WHITGL_SHADER_MODEL,
	WHITGL_SHADER_POST,
	WHITGL_SHADER_MAX,
} whitgl_shader_slot;

#define WHITGL_MAX_SHADER_UNIFORMS (16)
#define WHITGL_MAX_SHADER_COLORS (4)
#define WHITGL_MAX_SHADER_IMAGES (4)
typedef struct
{
	const char* vertex_src;
	const char* fragment_src;
	int num_uniforms;
	const char* uniforms[WHITGL_MAX_SHADER_UNIFORMS];
	int num_colors;
	const char* colors[WHITGL_MAX_SHADER_COLORS];
	int num_images;
	const char* images[WHITGL_MAX_SHADER_IMAGES];
} whitgl_shader;
static const whitgl_shader whitgl_shader_zero = {NULL, NULL, 0, {}, 0, {}, 0, {}};

bool whitgl_sys_init(whitgl_sys_setup* setup);
bool whitgl_sys_should_close();
void whitgl_sys_close();

void whitgl_sys_set_clear_color(whitgl_sys_color col);

bool whitgl_change_shader(whitgl_shader_slot type, whitgl_shader shader);
void whitgl_set_shader_uniform(whitgl_shader_slot type, int uniform, float value);
void whitgl_set_shader_color(whitgl_shader_slot type, whitgl_int color, whitgl_sys_color value);
void whitgl_set_shader_image(whitgl_shader_slot type, whitgl_int image, whitgl_int index);

void whitgl_sys_draw_init();
void whitgl_sys_draw_finish();

void whitgl_sys_add_image_from_data(int id, whitgl_ivec size, unsigned char* data);
void whitgl_sys_update_image_from_data(int id, whitgl_ivec size, unsigned char* data);
bool whitgl_sys_load_png(const char *name, whitgl_int *width, whitgl_int *height, unsigned char **data);
bool whitgl_sys_save_png(const char *name, whitgl_int width, whitgl_int height, unsigned char *data);
void whitgl_sys_capture_frame(const char *name);
void whitgl_sys_add_image(int id, const char* filename);
void whitgl_sys_image_from_data(int id, whitgl_ivec size, const unsigned char* data);
void whitgl_sys_draw_iaabb(whitgl_iaabb rectangle, whitgl_sys_color col);
void whitgl_sys_draw_hollow_iaabb(whitgl_iaabb rect, whitgl_int width, whitgl_sys_color col);
void whitgl_sys_draw_line(whitgl_iaabb line, whitgl_sys_color col);
void whitgl_sys_draw_fcircle(whitgl_fcircle circle, whitgl_sys_color col, int tris);
void whitgl_sys_draw_tex_iaabb(int id, whitgl_iaabb src, whitgl_iaabb dest);
void whitgl_sys_draw_sprite(whitgl_sprite sprite, whitgl_ivec frame, whitgl_ivec pos);
void whitgl_sys_draw_sprite_sized(whitgl_sprite sprite, whitgl_ivec frame, whitgl_ivec pos, whitgl_ivec dest_size);
void whitgl_sys_draw_text(whitgl_sprite sprite, const char* string, whitgl_ivec pos);

void whitgl_sys_draw_model(whitgl_int id, whitgl_fmat matrix);
whitgl_bool whitgl_load_model(whitgl_int id, const char* filename);

double whitgl_sys_get_time();

whitgl_sys_color whitgl_sys_color_blend(whitgl_sys_color a, whitgl_sys_color b, whitgl_float factor);
whitgl_sys_color whitgl_sys_color_multiply(whitgl_sys_color a, whitgl_sys_color b);

whitgl_bool whitgl_sys_window_focused();

#endif // WHITGL_SYS_H_
