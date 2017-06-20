#ifndef WHITGL_PROFILE_H_
#define WHITGL_PROFILE_H_

#include <whitgl/math.h>

typedef enum
{
	PROFILE_START_FRAME,
	PROFILE_START_GRAPHICS,
	PROFILE_FINISH_FRAME,
} whitgl_profile_event;

void whitgl_profile_init();
void whitgl_profile_start_frame();
void whitgl_profile_start_drawing();
void whitgl_profile_end_frame();
void whitgl_profile_shutdown();

#endif // WHITGL_PROFILE_H_
