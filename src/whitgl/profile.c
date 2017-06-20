#include <whitgl/logging.h>
#include <whitgl/profile.h>
#include <whitgl/sys.h>

#define GLEW_STATIC
#include <GL/glew.h>

whitgl_float _frame_start;
whitgl_float _frame_total;
whitgl_int _frames;

#define FRAMES_TO_COUNT (60)

void whitgl_profile_init()
{
	// GLuint queries[2];
	// glGenQueries(2, queries);
}
void whitgl_profile_start_frame()
{
	_frame_start = whitgl_sys_get_time();
}
void whitgl_profile_start_drawing()
{
	// glBeginQuery(GL_TIME_ELAPSED, queries[0]);
}
void whitgl_profile_end_frame()
{
	whitgl_float _frame_end = whitgl_sys_get_time();
	_frame_total += _frame_end-_frame_start;
	_frames++;
	if(_frames >= FRAMES_TO_COUNT)
	{
		_frames -= FRAMES_TO_COUNT;
		whitgl_float average_frame = _frame_total/FRAMES_TO_COUNT;
		whitgl_float target = 1.0/60.0;
		WHITGL_LOG("total %.2f%%", (average_frame/target)*100);
		_frame_total = 0;
	}
}
void whitgl_profile_shutdown()
{
}
