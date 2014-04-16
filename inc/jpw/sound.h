
#define SOUND_MAX (64)

void jpw_sound_init();
void jpw_sound_update();
void jpw_sound_shutdown();
void jpw_sound_add(int id, const char* filename);
void jpw_sound_play(int id, float adjust);
