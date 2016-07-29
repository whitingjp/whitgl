#ifndef WHITGL_DEBUG_H_
#define WHITGL_DEBUG_H_

#include <whitgl/sys.h>

typedef enum
{
	WHITGL_DEBUG_INT,
	WHITGL_DEBUG_FLOAT,
	WHITGL_DEBUG_BUTTON,
} whitgl_debug_menu_type;

typedef struct
{
	whitgl_debug_menu_type type;
	const char* name;
	const whitgl_int* int_val;
	const whitgl_float* float_val;
	whitgl_bool* bool_val;
} whitgl_debug_menu_entry;
static const whitgl_debug_menu_entry whitgl_debug_menu_entry_zero = {WHITGL_DEBUG_INT, NULL, NULL, NULL, NULL};

#define WHITGL_DEBUG_MAX_ENTRIES (8)
typedef struct
{
	int num_entries;
	whitgl_debug_menu_entry entries[WHITGL_DEBUG_MAX_ENTRIES];
	whitgl_ivec pos;
	whitgl_int selected;
	whitgl_int pixel_size;
	whitgl_sprite text_sprite;
} whitgl_debug_menu;
whitgl_debug_menu whitgl_debug_menu_zero(whitgl_ivec pos, whitgl_sprite text_sprite, whitgl_int pixel_size);

whitgl_debug_menu whitgl_debug_menu_update(whitgl_debug_menu debug);

whitgl_debug_menu whitgl_debug_menu_add_int(whitgl_debug_menu debug, const char* name, const whitgl_int* val);
whitgl_debug_menu whitgl_debug_menu_add_float(whitgl_debug_menu debug, const char* name, const whitgl_float* val);
whitgl_debug_menu whitgl_debug_menu_add_button(whitgl_debug_menu debug, const char* name, whitgl_bool* trigger);
void whitgl_debug_menu_draw(whitgl_debug_menu debug);

#endif // WHITGL_DEBUG_H_
