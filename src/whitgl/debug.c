#include <whitgl/debug.h>
#include <whitgl/input.h>
#include <whitgl/logging.h>

#include <stdio.h>
#include <string.h>

whitgl_debug_menu whitgl_debug_menu_zero(whitgl_ivec pos, whitgl_sprite text_sprite, whitgl_int pixel_size)
{
	whitgl_debug_menu debug;
	debug.active = false;
	whitgl_int i;
	for(i=0; i<WHITGL_DEBUG_MAX_ENTRIES; i++)
	{
		debug.entries[i] = whitgl_debug_menu_entry_zero;
	}
	debug.num_entries = 0;
	debug.pos = pos;
	debug.selected = -1;
	debug.pixel_size = pixel_size;
	debug.text_sprite = text_sprite;
	return debug;
}

whitgl_debug_menu whitgl_debug_menu_update(whitgl_debug_menu debug)
{
	whitgl_int i;
	if(whitgl_input_pressed(WHITGL_INPUT_DEBUG))
		debug.active = !debug.active;
	if(!debug.active)
		return debug;
	for(i=0; i<WHITGL_DEBUG_MAX_ENTRIES; i++)
	{
		if(debug.entries[i].type == WHITGL_DEBUG_BUTTON)
			*debug.entries[i].bool_val = false;
	}
	whitgl_ivec mouse_pos = whitgl_input_mouse_pos(debug.pixel_size);
	mouse_pos = whitgl_ivec_sub(mouse_pos, debug.pos);
	debug.selected = mouse_pos.y/debug.text_sprite.size.y;
	if(debug.selected < 0 || debug.selected > debug.num_entries-1)
		debug.selected = -1;
	if(mouse_pos.x < 0 || mouse_pos.x > debug.text_sprite.size.x*11)
		debug.selected = -1;
	if(debug.entries[debug.selected].type != WHITGL_DEBUG_BUTTON)
		debug.selected = -1;
	if(debug.selected == -1)
		return debug;
	if(!whitgl_input_pressed(WHITGL_INPUT_MOUSE_LEFT))
		return debug;
	*debug.entries[debug.selected].bool_val = true;
	return debug;
}

whitgl_debug_menu whitgl_debug_menu_add_int(whitgl_debug_menu debug, const char* name, const whitgl_int* val)
{
	if(debug.num_entries >= WHITGL_DEBUG_MAX_ENTRIES)
		WHITGL_PANIC("run out of debug entries");
	whitgl_int index = debug.num_entries;
	debug.entries[index].type = WHITGL_DEBUG_INT;
	debug.entries[index].name = name;
	debug.entries[index].int_val = val;
	debug.num_entries++;
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
whitgl_debug_menu whitgl_debug_menu_add_button(whitgl_debug_menu debug, const char* name, whitgl_bool* trigger)
{
	if(debug.num_entries >= WHITGL_DEBUG_MAX_ENTRIES)
		WHITGL_PANIC("run out of debug entries");
	whitgl_int index = debug.num_entries;
	debug.entries[index].type = WHITGL_DEBUG_BUTTON;
	debug.entries[index].name = name;
	debug.entries[index].bool_val = trigger;
	debug.num_entries++;
	return debug;
}


void whitgl_debug_menu_draw(whitgl_debug_menu debug)
{
	if(!debug.active)
		return;
	whitgl_ivec draw_pos = debug.pos;
	whitgl_int i;
	for(i=0; i<debug.num_entries; i++)
	{
		char buffer[64];
		buffer[0] = '\0';
		switch(debug.entries[i].type)
		{
			case WHITGL_DEBUG_INT:
			{
				snprintf(buffer, sizeof(buffer), "%5s %5d", debug.entries[i].name, (int)*debug.entries[i].int_val);
				break;
			}
			case WHITGL_DEBUG_FLOAT:
			{
				snprintf(buffer, sizeof(buffer), "%5s %5.1f", debug.entries[i].name, *debug.entries[i].float_val);
				break;
			}
			case WHITGL_DEBUG_BUTTON:
			{
				snprintf(buffer, sizeof(buffer), "%11s", debug.entries[i].name);
				break;
			}
		}
		whitgl_iaabb backing;
		backing.a.x = draw_pos.x-1;
		backing.a.y = draw_pos.y;
		if(i==0)
			backing.a.y--;
		backing.b.x = draw_pos.x+strlen(buffer)*debug.text_sprite.size.x;
		backing.b.y = draw_pos.y+debug.text_sprite.size.y;
		whitgl_sys_color darken = {0x00,0x00,0x00,0x60};
		whitgl_sys_color selected = {0x00,0x00,0x00,0xff};
		if(debug.selected == i)
			whitgl_sys_draw_iaabb(backing, selected);
		else
			whitgl_sys_draw_iaabb(backing, darken);
		whitgl_sys_draw_text(debug.text_sprite, buffer, draw_pos);
		draw_pos.y += debug.text_sprite.size.y;
	}
}
