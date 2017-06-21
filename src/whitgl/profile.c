#include <whitgl/logging.h>
#include <whitgl/profile.h>
#include <whitgl/sys.h>

#define GLEW_STATIC
#include <GL/glew.h>

whitgl_float _frame_start;
whitgl_float _frame_update;
whitgl_float _frame_total;
whitgl_float _frame_gpu;
whitgl_int _frames;
#define NUM_QUERIES (1)
GLuint _front_queries[NUM_QUERIES];
GLuint _back_queries[NUM_QUERIES];
whitgl_bool _front;
whitgl_bool _first;

#define FRAMES_TO_COUNT (60)

void whitgl_profile_init()
{
	glGenQueries(NUM_QUERIES, _front_queries);
	glGenQueries(NUM_QUERIES, _back_queries);
	_first = true;
	_front = true;
}
void whitgl_profile_start_frame()
{
	_frame_start = whitgl_sys_get_time();
}
void whitgl_profile_start_drawing()
{
	_frame_update += whitgl_sys_get_time()-_frame_start;

	glBeginQuery(GL_TIME_ELAPSED, _front ? _front_queries[0] : _back_queries[0]);
}
void whitgl_profile_end_frame()
{
	glEndQuery(GL_TIME_ELAPSED);
	if(!_first)
	{
		int done = 0;
		while (!done)
		{
			glGetQueryObjectiv(!_front ? _front_queries[0] : _back_queries[0], GL_QUERY_RESULT_AVAILABLE, &done);
		}
		GLuint64 timer;
		glGetQueryObjectui64v(!_front ? _front_queries[0] : _back_queries[0], GL_QUERY_RESULT, &timer);
		_frame_gpu += timer / 1000000000.0;
	}
	_front = !_front;
	_first = false;
	_frame_total += whitgl_sys_get_time()-_frame_start;
	_frames++;
	if(_frames >= FRAMES_TO_COUNT)
	{
		_frames -= FRAMES_TO_COUNT;
		whitgl_float average_gpu = _frame_gpu/FRAMES_TO_COUNT;
		whitgl_float average_total = _frame_total/FRAMES_TO_COUNT;
		whitgl_float average_update = _frame_update/FRAMES_TO_COUNT;
		whitgl_float target = 1.0/60.0;
		WHITGL_LOG("update %.2f%% gpu %.2f%% total %.2f%%", (average_update/target)*100, (average_gpu/target)*100, (average_total/target)*100);
		_frame_update = 0;
		_frame_total = 0;
		_frame_gpu = 0;
	}
}
void whitgl_profile_shutdown()
{
	glDeleteQueries(NUM_QUERIES, _front_queries);
	glDeleteQueries(NUM_QUERIES, _back_queries);
}
