#include <fmod.h>
#include <fmod_errors.h>
#include <stdbool.h>
#include <stddef.h>

#include <whitgl/logging.h>
#include <whitgl/sound.h>

FMOD_SYSTEM *fmodSystem;
typedef struct
{
	int id;
	FMOD_SOUND* sound;
	FMOD_CHANNEL* channel;
} whitgl_sound;
#define WHITGL_SOUND_MAX (64)
whitgl_sound sounds[WHITGL_SOUND_MAX];
int num_sounds;

void _whitgl_sound_errcheck(const char* location, FMOD_RESULT result)
{
    if (result != FMOD_OK)
    	WHITGL_LOG("FMOD error in %s! (%d) %s\n", location, result, FMOD_ErrorString(result));
}

void whitgl_sound_init()
{
	int i;
	for(i=0; i<WHITGL_SOUND_MAX; i++)
		sounds[i].id = -1;
	num_sounds = 0;

	WHITGL_LOG("Initialize fmod");
	fmodSystem = NULL;

	FMOD_RESULT result;
	result = FMOD_System_Create(&fmodSystem);
	_whitgl_sound_errcheck("FMOD_System_Create", result);

	result = FMOD_System_Init(fmodSystem, 1024, FMOD_INIT_NORMAL, NULL);
	_whitgl_sound_errcheck("FMOD_System_Init", result);


}
void whitgl_sound_update()
{
	FMOD_System_Update(fmodSystem);
}
void whitgl_sound_shutdown()
{
	WHITGL_LOG("Shutdown fmod");
	FMOD_RESULT result;
	int i;
	for(i=0; i<num_sounds; i++)
		FMOD_Sound_Release(sounds[i].sound);
	result = FMOD_System_Close(fmodSystem);
	_whitgl_sound_errcheck("FMOD_System_Close", result);
	result = FMOD_System_Release(fmodSystem);
	_whitgl_sound_errcheck("FMOD_System_Release", result);
}
void whitgl_sound_add(int id, const char* filename)
{
	WHITGL_LOG("Adding sound: %s", filename);
	if(num_sounds >= WHITGL_SOUND_MAX)
	{
		WHITGL_LOG("ERR Too many sounds");
		return;
	}
	sounds[num_sounds].id = id;
	FMOD_RESULT result = FMOD_System_CreateSound(fmodSystem, filename, FMOD_CREATESAMPLE | FMOD_2D, 0, &sounds[num_sounds].sound);
	_whitgl_sound_errcheck("FMOD_System_CreateSound", result);
	num_sounds++;
}
int _whitgl_get_index(int id)
{
	int index = -1;
	int i;
	for(i=0; i<num_sounds; i++)
	{
		if(sounds[i].id == id)
		{
			index = i;
			continue;
		}
	}
	if(index == -1)
		WHITGL_PANIC("ERR Cannot find sound %d", id);
	return index;
}
void whitgl_sound_play(int id, float adjust)
{
	int index = _whitgl_get_index(id);
	FMOD_RESULT result = FMOD_System_PlaySound(fmodSystem, sounds[index].sound, NULL, true, &sounds[index].channel);
	_whitgl_sound_errcheck("FMOD_System_PlaySound", result);

	float defaultFrequency;
	result = FMOD_Sound_GetDefaults(sounds[index].sound, &defaultFrequency, NULL);
	_whitgl_sound_errcheck("FMOD_Sound_GetDefaults", result);
	result = FMOD_Channel_SetFrequency(sounds[index].channel, defaultFrequency*adjust);
	_whitgl_sound_errcheck("FMOD_Channel_SetFrequency", result);
	result = FMOD_Channel_SetPaused(sounds[index].channel, false);
	_whitgl_sound_errcheck("FMOD_Channel_SetPaused", result);
}

void whitgl_loop_add(int id, const char* filename, whitgl_sound_start_state state)
{
	WHITGL_LOG("Adding loop: %s", filename);
	if(num_sounds >= WHITGL_SOUND_MAX)
	{
		WHITGL_LOG("ERR Too many sounds");
		return;
	}
	sounds[num_sounds].id = id;
	FMOD_RESULT result = FMOD_System_CreateSound(fmodSystem, filename, FMOD_CREATESTREAM | FMOD_2D | FMOD_LOOP_NORMAL, 0, &sounds[num_sounds].sound);
	_whitgl_sound_errcheck("FMOD_System_CreateSound", result);

	result = FMOD_System_PlaySound(fmodSystem, sounds[num_sounds].sound, NULL, true, &sounds[num_sounds].channel);
	_whitgl_sound_errcheck("FMOD_System_PlaySound", result);

	result = FMOD_Channel_SetVolume(sounds[num_sounds].channel, 0);
	_whitgl_sound_errcheck("FMOD_Channel_SetVolume", result);

	if(state == WHITGL_SOUND_START_UNPAUSED)
	{
		result = FMOD_Channel_SetPaused(sounds[num_sounds].channel, false);
		_whitgl_sound_errcheck("FMOD_Channel_SetPaused", result);
	}

	num_sounds++;
}
void whitgl_loop_volume(int id, float volume)
{
	int index = _whitgl_get_index(id);
	FMOD_RESULT result = FMOD_Channel_SetVolume(sounds[index].channel, volume);
	_whitgl_sound_errcheck("FMOD_Channel_SetVolume", result);
}
void whitgl_loop_unpause(int id)
{
	int index = _whitgl_get_index(id);
	FMOD_RESULT result = FMOD_Channel_SetPaused(sounds[index].channel, false);
	_whitgl_sound_errcheck("FMOD_Channel_SetPaused", result);
}
void whitgl_loop_seek(int id, float time)
{
	int index = _whitgl_get_index(id);
	FMOD_RESULT result = FMOD_Channel_SetPosition(sounds[index].channel, time*1000, FMOD_TIMEUNIT_MS);
	_whitgl_sound_errcheck("FMOD_Channel_SetPosition", result);
}
void whitgl_loop_frequency(int id, float adjust)
{
	int index = _whitgl_get_index(id);
	float defaultFrequency;
	FMOD_RESULT result;
	result = FMOD_Sound_GetDefaults(sounds[index].sound, &defaultFrequency, NULL);
	_whitgl_sound_errcheck("FMOD_Sound_GetDefaults", result);
	result = FMOD_Channel_SetFrequency(sounds[index].channel, defaultFrequency*adjust);
	_whitgl_sound_errcheck("FMOD_Channel_SetFrequency", result);
}