#ifndef WHITGL_DEBUG_H_
#define WHITGL_DEBUG_H_

#include <whitgl/sys.h>

typedef enum
{
	WHITGL_DEBUG_FLOAT,
} whitgl_debug_menu_type;

typedef struct
{
	whitgl_debug_menu_type type;
	const char* name;
	const whitgl_float* float_val;
} whitgl_debug_menu_entry;
static const whitgl_debug_menu_entry whitgl_debug_menu_entry_zero = {WHITGL_DEBUG_FLOAT, NULL, NULL};

#define WHITGL_DEBUG_MAX_ENTRIES (8)
typedef struct
{
	int num_entries;
	whitgl_debug_menu_entry entries[WHITGL_DEBUG_MAX_ENTRIES];
} whitgl_debug_menu;
whitgl_debug_menu whitgl_debug_menu_zero();

whitgl_debug_menu whitgl_debug_menu_add_float(whitgl_debug_menu debug, const char* name, const whitgl_float* val);
void whitgl_debug_menu_draw(whitgl_debug_menu debug, whitgl_sprite text_sprite, whitgl_ivec draw_pos);

#endif // WHITGL_DEBUG_H_
