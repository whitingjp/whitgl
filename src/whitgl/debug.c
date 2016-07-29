#include <whitgl/debug.h>
#include <whitgl/logging.h>

#include <stdio.h>
#include <string.h>

whitgl_debug_menu whitgl_debug_menu_zero()
{
	whitgl_debug_menu debug;
	whitgl_int i;
	for(i=0; i<WHITGL_DEBUG_MAX_ENTRIES; i++)
	{
		debug.entries[i] = whitgl_debug_menu_entry_zero;
	}
	debug.num_entries = 0;
	return debug;
}
whitgl_debug_menu whitgl_debug_menu_add_float(whitgl_debug_menu debug, const char* name, const whitgl_float* val)
{
	if(debug.num_entries >= WHITGL_DEBUG_MAX_ENTRIES)
		WHITGL_PANIC("run out of debug entries");
	whitgl_int index = debug.num_entries;
	debug.entries[index].type = WHITGL_DEBUG_FLOAT;
	debug.entries[index].name = name;
	debug.entries[index].float_val = val;
	debug.num_entries++;
	return debug;
}
void whitgl_debug_menu_draw(whitgl_debug_menu debug, whitgl_sprite text_sprite, whitgl_ivec draw_pos)
{
	whitgl_int i;
	for(i=0; i<debug.num_entries; i++)
	{
		char buffer[64];
		buffer[0] = '\0';
		switch(debug.entries[i].type)
		{
			case WHITGL_DEBUG_FLOAT:
			{
     			snprintf(buffer, sizeof(buffer), "%5s %5.1f", debug.entries[i].name, *debug.entries[i].float_val);
				break;
			}
		}
		whitgl_iaabb backing;
		backing.a.x = draw_pos.x-1;
		backing.a.y = draw_pos.y;
		if(i==0)
			backing.a.y--;
		backing.b.x = draw_pos.x+strlen(buffer)*text_sprite.size.x;
		backing.b.y = draw_pos.y+text_sprite.size.y;
		whitgl_sys_color darken = {0x00,0x00,0x00,0x60};
		whitgl_sys_draw_iaabb(backing, darken);
		whitgl_sys_draw_text(text_sprite, buffer, draw_pos);
		draw_pos.y += text_sprite.size.y;
	}
}
