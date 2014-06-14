#ifndef WHITGL_SOUND_H_
#define WHITGL_SOUND_H_

void whitgl_sound_init();
void whitgl_sound_update();
void whitgl_sound_shutdown();
void whitgl_sound_add(int id, const char* filename);
void whitgl_sound_play(int id, float adjust);
void whitgl_loop_add(int id, const char* filename);
void whitgl_loop_volume(int id, float volume);

#endif // WHITGL_SOUND_H_