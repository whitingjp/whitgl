#ifndef WHITGL_SOUND_H_
#define WHITGL_SOUND_H_

typedef enum
{
	WHITGL_SOUND_PAUSED,
	WHITGL_SOUND_UNPAUSED,
} whitgl_sound_state;

void whitgl_sound_init();
void whitgl_sound_shutdown();
void whitgl_sound_update();
void whitgl_sound_volume(float volume);

void whitgl_sound_add(int id, const char* filename);
void whitgl_sound_play(int id, float adjust);
void whitgl_loop_add(int id, const char* filename);
void whitgl_loop_volume(int id, float volume);
void whitgl_loop_set_paused(int id, bool paused);
void whitgl_loop_seek(int id, float time);
void whitgl_loop_frequency(int id, float adjust);

#endif // WHITGL_SOUND_H_