#include <whitgl/logging.h>
#include <whitgl/timer.h>
#include <whitgl/sys.h>

whitgl_float _whitgl_timer_now;
whitgl_float _whitgl_timer_then;
whitgl_float _whitgl_timer_dt;
whitgl_int _whitgl_timer_frames;
whitgl_float _whitgl_timer_fps_timer;
whitgl_int _whitgl_timer_fps;

void whitgl_timer_init()
{
	_whitgl_timer_now = whitgl_sys_get_time();
	_whitgl_timer_then = _whitgl_timer_now;
	_whitgl_timer_frames = 0;
	_whitgl_timer_fps_timer = 0;
	_whitgl_timer_fps = 60;
}
whitgl_float whitgl_timer_next_frame()
{
	_whitgl_timer_then = _whitgl_timer_now;
	_whitgl_timer_now = whitgl_sys_get_time();
	whitgl_float dt = _whitgl_timer_now - _whitgl_timer_then;
	if(dt > 1.0/30.0)
		dt = 1.0/30.0;
	_whitgl_timer_frames++;
	_whitgl_timer_fps_timer += _whitgl_timer_now - _whitgl_timer_then;
	if(_whitgl_timer_fps_timer >= 1)
	{
		_whitgl_timer_fps = _whitgl_timer_frames;
		if(_whitgl_timer_frames < 55)
			WHITGL_LOG("fps: %d", _whitgl_timer_fps);
		_whitgl_timer_fps_timer -= 1;
		_whitgl_timer_frames = 0;
	}
	return dt;
}
whitgl_int whitgl_timer_fps()
{
	return _whitgl_timer_fps;
}
