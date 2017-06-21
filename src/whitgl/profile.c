#include <whitgl/logging.h>
#include <whitgl/profile.h>
#include <whitgl/sys.h>

#define GLEW_STATIC
#include <GL/glew.h>


typedef struct
{
	const char* name;
	whitgl_float total;
} whitgl_profile_event;
static const whitgl_profile_event whitgl_profile_event_zero = {NULL,0};
#define MAX_EVENTS (16)
whitgl_profile_event gpu_events[MAX_EVENTS];
whitgl_int next_gpu_event;

whitgl_float _frame_start;
whitgl_float _frame_update;
whitgl_float _frame_total;
whitgl_int _frames;
GLuint _front_queries[MAX_EVENTS];
GLuint _back_queries[MAX_EVENTS];
whitgl_bool _front;
whitgl_bool _first;
whitgl_bool _should_report;

#define FRAMES_TO_COUNT (60)

void whitgl_profile_init()
{
	glGenQueries(MAX_EVENTS, _front_queries);
	glGenQueries(MAX_EVENTS, _back_queries);
	_first = true;
	_front = true;
	_should_report = false;
}
void whitgl_profile_start_frame()
{
	_frame_start = whitgl_sys_get_time();
}
void whitgl_profile_start_drawing()
{
	_frame_update += whitgl_sys_get_time()-_frame_start;
	next_gpu_event = 0;
	whitgl_profile_gpu_section("start_drawing");
}

void whitgl_profile_gpu_section(const char* name)
{
	if(next_gpu_event > 0)
		glEndQuery(GL_TIME_ELAPSED);
	gpu_events[next_gpu_event].name = name;
	glBeginQuery(GL_TIME_ELAPSED, _front ? _front_queries[next_gpu_event] : _back_queries[next_gpu_event]);
	next_gpu_event++;
}

void whitgl_profile_end_frame()
{
	glEndQuery(GL_TIME_ELAPSED);
	if(!_first)
	{
		GLuint* query_array = !_front ? _front_queries : _back_queries;
		int done = 0;
		while (!done)
		{
			glGetQueryObjectiv(query_array[next_gpu_event-1], GL_QUERY_RESULT_AVAILABLE, &done);
		}
		whitgl_int i;
		for(i=0; i<next_gpu_event; i++)
		{
			GLuint64 timer;
			glGetQueryObjectui64v(query_array[i], GL_QUERY_RESULT, &timer);
			gpu_events[i].total += timer / 1000000000.0;
		}
	}
	_front = !_front;
	_first = false;
	_frame_total += whitgl_sys_get_time()-_frame_start;
	_frames++;
	if(_frames >= FRAMES_TO_COUNT)
	{
		whitgl_float target = 1.0/60.0;
		_frames -= FRAMES_TO_COUNT;
		whitgl_int i;
		whitgl_float average_gpu = 0;
		for(i=0; i<next_gpu_event; i++)
		{
			whitgl_float event_avg = gpu_events[i].total/FRAMES_TO_COUNT;
			average_gpu += event_avg;
			if(_should_report)
				WHITGL_LOG("%.1f%% %s", (event_avg/target)*100, gpu_events[i].name);
			gpu_events[i] = whitgl_profile_event_zero;
		}
		whitgl_float average_total = _frame_total/FRAMES_TO_COUNT;
		whitgl_float average_update = _frame_update/FRAMES_TO_COUNT;
		if(_should_report)
			WHITGL_LOG("update %.2f%% gpu %.2f%% total %.2f%%", (average_update/target)*100, (average_gpu/target)*100, (average_total/target)*100);
		_frame_update = 0;
		_frame_total = 0;
	}
}
void whitgl_profile_should_report(whitgl_bool report)
{
	_should_report = report;
}
void whitgl_profile_shutdown()
{
	glDeleteQueries(MAX_EVENTS, _front_queries);
	glDeleteQueries(MAX_EVENTS, _back_queries);
}
