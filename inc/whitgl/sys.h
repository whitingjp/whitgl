#ifndef WHITGL_SYS_H_
#define WHITGL_SYS_H_

#include <stdbool.h>
#include <stddef.h>

#include <whitgl/math.h>

typedef enum
{
	CURSOR_SHOW,
	CURSOR_HIDE,
	CURSOR_DISABLE,
} whitgl_sys_cursor_mode;

typedef enum
{
	RESOLUTION_AT_LEAST,
	RESOLUTION_AT_MOST,
	RESOLUTION_EXACT,
	RESOLUTION_USE_WINDOW,
} whitgl_sys_resolution_mode;

typedef struct
{
	const char* name;
	whitgl_ivec size;
	int pixel_size;
	whitgl_sys_resolution_mode resolution_mode;
	whitgl_bool fullscreen;
	whitgl_sys_cursor_mode cursor;
	whitgl_bool vsync;
	whitgl_bool start_focused;
	whitgl_bool clear_buffer;
	whitgl_int num_framebuffers;
} whitgl_sys_setup;
static const whitgl_sys_setup whitgl_sys_setup_zero =
{
	"default window name",
	{120, 80},
	4,
	RESOLUTION_AT_LEAST,
	false,
	CURSOR_SHOW,
	true,
	true,
	true,
	1,
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
	WHITGL_SHADER_EXTRA_0,
	WHITGL_SHADER_EXTRA_1,
	WHITGL_SHADER_EXTRA_2,
	WHITGL_SHADER_EXTRA_3,
	WHITGL_SHADER_EXTRA_4,
	WHITGL_SHADER_EXTRA_5,
	WHITGL_SHADER_EXTRA_6,
	WHITGL_SHADER_EXTRA_7,
	WHITGL_SHADER_MAX,
} whitgl_shader_slot;

typedef enum
{
	WHITGL_UNIFORM_FLOAT,
	WHITGL_UNIFORM_FVEC,
	WHITGL_UNIFORM_FVEC3,
	WHITGL_UNIFORM_COLOR,
	WHITGL_UNIFORM_IMAGE,
	WHITGL_UNIFORM_FRAMEBUFFER,
	WHITGL_UNIFORM_MATRIX,
} whitgl_uniform_type;
#define WHITGL_MAX_SHADER_UNIFORMS (32)
typedef struct
{
	whitgl_uniform_type type;
	const char* name;
} whitgl_uniform;
typedef struct
{
	const char* vertex_src;
	const char* fragment_src;
	int num_uniforms;
	whitgl_uniform uniforms[WHITGL_MAX_SHADER_UNIFORMS];
} whitgl_shader;
static const whitgl_shader whitgl_shader_zero = {NULL, NULL, 0, {}};

bool whitgl_sys_init(whitgl_sys_setup* setup);
bool whitgl_sys_should_close();
void whitgl_sys_close();

void whitgl_sys_set_clear_color(whitgl_sys_color col);

bool whitgl_change_shader(whitgl_shader_slot type, whitgl_shader shader);
void whitgl_set_shader_float(whitgl_shader_slot type, whitgl_int uniform, float value);
void whitgl_set_shader_fvec(whitgl_shader_slot type, whitgl_int uniform, whitgl_fvec value);
void whitgl_set_shader_fvec3(whitgl_shader_slot type, whitgl_int uniform, whitgl_fvec3 value);
void whitgl_set_shader_color(whitgl_shader_slot type, whitgl_int uniform, whitgl_sys_color value);
void whitgl_set_shader_image(whitgl_shader_slot type, whitgl_int uniform, whitgl_int index);
void whitgl_set_shader_framebuffer(whitgl_shader_slot type, whitgl_int uniform, whitgl_int index);
void whitgl_set_shader_matrix(whitgl_shader_slot type, whitgl_int matrix, whitgl_fmat fmat);

void whitgl_sys_draw_init(whitgl_int framebuffer_id);
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
void whitgl_sys_draw_buffer_pane(whitgl_int id, whitgl_fvec3 verts[4], whitgl_shader_slot shader, whitgl_fmat m_model, whitgl_fmat m_view, whitgl_fmat m_perspective);
void whitgl_resize_framebuffer(whitgl_int i, whitgl_ivec size, whitgl_bool one_color);

void whitgl_sys_draw_model(whitgl_int id, whitgl_shader_slot shader, whitgl_fmat m_model, whitgl_fmat m_view, whitgl_fmat m_perspective);
void whitgl_sys_update_model_from_data(int id, whitgl_int num_vertices, const char* data);
whitgl_bool whitgl_load_model(whitgl_int id, const char* filename);

whitgl_ivec whitgl_sys_get_image_size(whitgl_int id);

whitgl_float whitgl_sys_get_time();

whitgl_sys_color whitgl_sys_color_blend(whitgl_sys_color a, whitgl_sys_color b, whitgl_float factor);
whitgl_sys_color whitgl_sys_color_multiply(whitgl_sys_color a, whitgl_sys_color b);

whitgl_bool whitgl_sys_window_focused();

void whitgl_sys_enable_depth(whitgl_bool enable);
void whitgl_sys_cull_side(whitgl_bool cull_front);

void whitgl_set_clipboard(const char* string);
const char* whitgl_get_clipboard();

#endif // WHITGL_SYS_H_
