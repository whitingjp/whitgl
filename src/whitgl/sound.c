#include <fmod.h>
#include <fmod_errors.h>
#include <stdbool.h>
#include <stddef.h>

#include <jpw/logging.h>
#include <jpw/sound.h>

FMOD_SYSTEM *fmodSystem;
typedef struct 
{
	int id;
	FMOD_SOUND* sound;
} jpw_sound;
jpw_sound sounds[SOUND_MAX];
int num_sounds;

void _jpw_sound_errcheck(const char* location, FMOD_RESULT result)
{
    if (result != FMOD_OK)
    	JPW_LOG("FMOD error in %s! (%d) %s\n", location, result, FMOD_ErrorString(result));
}

void jpw_sound_init()
{
	int i;
	for(i=0; i<SOUND_MAX; i++)
		sounds[i].id = -1;
	num_sounds = 0;

	JPW_LOG("Initialize fmod");
	fmodSystem = NULL;

	FMOD_RESULT result;
	result = FMOD_System_Create(&fmodSystem);
	_jpw_sound_errcheck("FMOD_System_Create", result);

	result = FMOD_System_Init(fmodSystem, 1024, FMOD_INIT_NORMAL, NULL);
	_jpw_sound_errcheck("FMOD_System_Init", result);


}
void jpw_sound_update()
{
	FMOD_System_Update(fmodSystem);
}
void jpw_sound_shutdown()
{
	JPW_LOG("Shutdown fmod");
	FMOD_RESULT result;
	int i;
	for(i=0; i<num_sounds; i++)
		FMOD_Sound_Release(sounds[i].sound);
	result = FMOD_System_Close(fmodSystem);
	_jpw_sound_errcheck("FMOD_System_Close", result);
	result = FMOD_System_Release(fmodSystem);
	_jpw_sound_errcheck("FMOD_System_Release", result);
}
void jpw_sound_add(int id, const char* filename)
{
	if(num_sounds >= SOUND_MAX)
	{
		JPW_LOG("ERR Too many sounds");
		return;
	}
	sounds[num_sounds].id = id;
	FMOD_RESULT result = FMOD_System_CreateSound(fmodSystem, filename, FMOD_HARDWARE | FMOD_2D, 0, &sounds[num_sounds].sound);
	_jpw_sound_errcheck("FMOD_System_CreateSound", result);
	num_sounds++;
}
void jpw_sound_play(int id, float adjust)
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
		JPW_LOG("ERR Cannot find sound %d", id);
		return;
	}

	FMOD_CHANNEL *channel;
	FMOD_RESULT result = FMOD_System_PlaySound(fmodSystem, FMOD_CHANNEL_FREE, sounds[index].sound, true, &channel);		
	_jpw_sound_errcheck("FMOD_System_PlaySound", result);

	float defaultFrequency;
	result = FMOD_Sound_GetDefaults(sounds[index].sound, &defaultFrequency, NULL, NULL, NULL);
	_jpw_sound_errcheck("FMOD_Sound_GetDefaults", result);
	result = FMOD_Channel_SetFrequency(channel, defaultFrequency*adjust);
	_jpw_sound_errcheck("FMOD_Channel_SetFrequency", result);
	result = FMOD_Channel_SetPaused(channel, false);
	_jpw_sound_errcheck("FMOD_Channel_SetPaused", result);
}