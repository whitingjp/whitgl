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
} whitgl_sound;
whitgl_sound sounds[SOUND_MAX];
int num_sounds;

void _whitgl_sound_errcheck(const char* location, FMOD_RESULT result)
{
    if (result != FMOD_OK)
    	WHITGL_LOG("FMOD error in %s! (%d) %s\n", location, result, FMOD_ErrorString(result));
}

void whitgl_sound_init()
{
	int i;
	for(i=0; i<SOUND_MAX; i++)
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
	if(num_sounds >= SOUND_MAX)
	{
		WHITGL_LOG("ERR Too many sounds");
		return;
	}
	sounds[num_sounds].id = id;
	FMOD_RESULT result = FMOD_System_CreateSound(fmodSystem, filename, FMOD_HARDWARE | FMOD_2D, 0, &sounds[num_sounds].sound);
	_whitgl_sound_errcheck("FMOD_System_CreateSound", result);
	num_sounds++;
}
void whitgl_sound_play(int id, float adjust)
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
	{
		WHITGL_LOG("ERR Cannot find sound %d", id);
		return;
	}

	FMOD_CHANNEL *channel;
	FMOD_RESULT result = FMOD_System_PlaySound(fmodSystem, FMOD_CHANNEL_FREE, sounds[index].sound, true, &channel);		
	_whitgl_sound_errcheck("FMOD_System_PlaySound", result);

	float defaultFrequency;
	result = FMOD_Sound_GetDefaults(sounds[index].sound, &defaultFrequency, NULL, NULL, NULL);
	_whitgl_sound_errcheck("FMOD_Sound_GetDefaults", result);
	result = FMOD_Channel_SetFrequency(channel, defaultFrequency*adjust);
	_whitgl_sound_errcheck("FMOD_Channel_SetFrequency", result);
	result = FMOD_Channel_SetPaused(channel, false);
	_whitgl_sound_errcheck("FMOD_Channel_SetPaused", result);
}