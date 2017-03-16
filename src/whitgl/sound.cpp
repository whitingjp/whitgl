
#include <irrKlang.h>
#include <stdbool.h>
#include <stddef.h>

extern "C"
{
#include <whitgl/math.h>
#include <whitgl/logging.h>
}
#include <whitgl/sound.h>


// FMOD_SYSTEM *fmodSystem;
// typedef struct
// {
// 	int id;
// 	FMOD_SOUND* sound;
// 	FMOD_CHANNEL* channel;
// } whitgl_sound;
// #define WHITGL_SOUND_MAX (64)
// whitgl_sound sounds[WHITGL_SOUND_MAX];
// int num_sounds;

// void _whitgl_sound_errcheck(const char* location, FMOD_RESULT result)
// {
//     if (result != FMOD_OK)
//     	WHITGL_LOG("FMOD error in %s! (%d) %s\n", location, result, FMOD_ErrorString(result));
// }

irrklang::ISoundEngine* irrklang_engine = NULL;
typedef struct
{
	int id;
	irrklang::ISoundSource* source;
} whitgl_sound;
typedef struct
{
	int id;
	irrklang::ISound* sound;
} whitgl_loop;
#define WHITGL_SOUND_MAX (64)
whitgl_sound sounds[WHITGL_SOUND_MAX];
whitgl_loop loops[WHITGL_SOUND_MAX];
int num_sounds;
int num_loops;

void whitgl_sound_init()
{
	irrklang_engine = irrklang::createIrrKlangDevice();
	if(!irrklang_engine)
		WHITGL_PANIC("Could not startup irrklang engine\n");
	num_sounds = 0;
}
void whitgl_sound_shutdown()
{
	if(!irrklang_engine)
		WHITGL_PANIC("whitgl_sound_shutdown without whitgl_sound_init?");
	irrklang_engine->drop();
}
void whitgl_sound_update()
{
}
void whitgl_sound_volume(float volume)
{
	irrklang_engine->setSoundVolume(volume);
}
void whitgl_sound_add(int id, const char* filename)
{
	if(num_sounds >= WHITGL_SOUND_MAX)
		WHITGL_PANIC("ran out of sounds");
	sounds[num_sounds].id = id;

	sounds[num_sounds].source = irrklang_engine->addSoundSourceFromFile(filename, irrklang::ESM_NO_STREAMING, true);
	num_sounds++;
}
int _whitgl_get_sound_index(int id)
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
void whitgl_sound_play(int id, float volume, float pitch)
{
	whitgl_int index = _whitgl_get_sound_index(id);
	irrklang::ISound* sound = irrklang_engine->play2D(sounds[index].source, false, true);
	sound->setVolume(volume);
	sound->setPlaybackSpeed(pitch);
	sound->setIsPaused(false);
	sound->drop();
}
int _whitgl_get_loop_index(int id)
{
	int index = -1;
	int i;
	for(i=0; i<num_sounds; i++)
	{
		if(loops[i].id == id)
		{
			index = i;
			continue;
		}
	}
	if(index == -1)
		WHITGL_PANIC("ERR Cannot find loop %d", id);
	return index;
}
void whitgl_loop_add(int id, const char* filename)
{
	if(num_loops >= WHITGL_SOUND_MAX)
		WHITGL_PANIC("ran out of loops");
	loops[num_loops].id = id;

	loops[num_loops].sound = irrklang_engine->play2D(filename, true, true);
	num_loops++;
}
void whitgl_loop_add_positional(int id, const char* filename)
{
	if(num_loops >= WHITGL_SOUND_MAX)
		WHITGL_PANIC("ran out of loops");
	loops[num_loops].id = id;

	irrklang::vec3df pos = irrklang::vec3df(0,0,0);
	loops[num_loops].sound = irrklang_engine->play3D(filename, pos, true, true);
}
void whitgl_loop_volume(int id, float volume)
{
	whitgl_int index = _whitgl_get_loop_index(id);
	loops[index].sound->setVolume(volume);
}
void whitgl_loop_set_paused(int id, bool paused)
{
	whitgl_int index = _whitgl_get_loop_index(id);
	loops[index].sound->setIsPaused(paused);
}
void whitgl_loop_seek(int id, float time)
{
	whitgl_int index = _whitgl_get_loop_index(id);
	if(!loops[index].sound->setPlayPosition(time*1000))
		WHITGL_PANIC("failed to seek");
}
void whitgl_loop_frequency(int id, float pitch)
{
	whitgl_int index = _whitgl_get_loop_index(id);
	loops[index].sound->setPlaybackSpeed(pitch);
}
void whitgl_loop_set_listener(whitgl_fvec p, whitgl_fvec v, whitgl_float angle)
{
	irrklang::vec3df pos = irrklang::vec3df(p.x, p.y, 0);
	irrklang::vec3df vel = irrklang::vec3df(v.x/60.0, v.y/60.0, 0);
	whitgl_fvec forward = whitgl_angle_to_fvec(-angle+whitgl_pi/2);
	irrklang::vec3df frw = irrklang::vec3df(forward.x, forward.y, 0.0f);
	irrklang::vec3df up = irrklang::vec3df(0,0,1);

	irrklang_engine->setListenerPosition(pos, frw, vel, up);
}
void whitgl_loop_set_position(int id, whitgl_fvec p, whitgl_fvec v)
{
	whitgl_int index = _whitgl_get_loop_index(id);
	irrklang::vec3df pos = irrklang::vec3df(p.x,p.y,0);
	irrklang::vec3df vel = irrklang::vec3df(v.x/60.0,v.y/60.0,0);
	loops[index].sound->setPosition(pos);
	loops[index].sound->setVelocity(vel);
}
