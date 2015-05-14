#ifndef WHITGL_TIMER_H_
#define WHITGL_TIMER_H_

#include <whitgl/math.h>

void whitgl_timer_init();
whitgl_float whitgl_timer_tick();
bool whitgl_timer_should_do_frame(whitgl_float fps);
whitgl_int whitgl_timer_fps();

#endif // WHITGL_TIMER_H_
