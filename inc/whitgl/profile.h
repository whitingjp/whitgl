#ifndef WHITGL_PROFILE_H_
#define WHITGL_PROFILE_H_

#include <whitgl/math.h>

void whitgl_profile_init();
void whitgl_profile_start_frame();
void whitgl_profile_start_drawing();
void whitgl_profile_gpu_section(const char* name);
void whitgl_profile_end_frame();
void whitgl_profile_should_report(whitgl_bool report);
void whitgl_profile_shutdown();

#endif // WHITGL_PROFILE_H_
