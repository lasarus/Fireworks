#include <SDL.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <limits.h>

ALCdevice * device;
ALCcontext * context;
ALuint buffers[2];

const int source_count = 16;
ALuint sources[source_count];

/* Load WAVE file via SDL functions*/
int load_wav(const char * filename,
	     ALuint *buffer)
{
  SDL_AudioSpec wav_spec;
  Uint32 wav_length = 0;
  Uint8 * wav_buffer = NULL;

  if(SDL_LoadWAV(filename, &wav_spec, &wav_buffer, &wav_length) != NULL)
    {
      alGenBuffers(1, buffer);
      alBufferData(*buffer, AL_FORMAT_MONO16, wav_buffer, wav_length, wav_spec.freq);
      SDL_FreeWAV(wav_buffer);
    }
  else
    return 1;
  return 0;
}

int init_sound()
{
  int i;
  device = alcOpenDevice(NULL);
  context = alcCreateContext(device, NULL);
  alcMakeContextCurrent(context);
  
  alListener3f(AL_POSITION, 0, 0, 0);
  alListener3f(AL_VELOCITY, 0, 0, 0);
  alListener3f(AL_ORIENTATION, 0, 0, -1);

  alGenSources(source_count, sources);

  if(load_wav("sound1.wav", &buffers[0]))
    return 1;
  if(load_wav("sound2.wav", &buffers[1]))
    return 1;

  for(i = 0; i < source_count; i++)
    {
      alSourcef(sources[i], AL_PITCH, 2);
      alSourcef(sources[i], AL_GAIN, 1);
      alSource3f(sources[i], AL_POSITION, 0, 0, 0);
      alSource3f(sources[i], AL_VELOCITY, 0, 0, 0);
      alSourcei(sources[i], AL_LOOPING, AL_FALSE);
      alSourcei(sources[i], AL_BUFFER, buffers[0]);
    }
  return 0;
}

int clean_sound()
{
  alDeleteSources(source_count, sources);
  alDeleteBuffers(2, buffers);
  device = alcGetContextsDevice(context);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);

  return 0; // There are never errors here
}

int rotation = 0;

void play_sound(int type)
{
  alSourcef(sources[rotation], AL_PITCH, 2 + (rand() / (double)RAND_MAX - .5));
  alSource3f(sources[rotation], AL_POSITION, 0, 0, 0);
  alSource3f(sources[rotation], AL_VELOCITY, 0, 0, 0);
  alSourcei(sources[rotation], AL_LOOPING, AL_FALSE);

  switch(type)
    {
    case 1:
      alSourcef(sources[rotation], AL_GAIN, .3);
      alSourcei(sources[rotation], AL_BUFFER, buffers[0]);
      break;

    case 2:
      alSourcef(sources[rotation], AL_GAIN, .3);
      alSourcei(sources[rotation], AL_BUFFER, buffers[1]);
      break;

    case 3:
      alSourcef(sources[rotation], AL_GAIN, 1);
      alSourcei(sources[rotation], AL_BUFFER, buffers[0]);
      break;

    default:
      return;
    }

  alSourcePlay(sources[rotation]);
  rotation++;
  rotation = rotation % source_count;
}
