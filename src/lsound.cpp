#include "lsound.h"

LSound::LSound()
{
    sound = NULL;
    chunkSound = NULL;
}

LSound::~LSound()
{
    free();
}

void LSound::free()
{
    Mix_FreeMusic(sound);
    sound = NULL;
    Mix_FreeChunk(chunkSound);
    chunkSound = NULL;
}

bool LSound::loadMusic(const std::string& path)
{
    sound = Mix_LoadMUS(path.c_str());
    bool success = true;
    if (sound == NULL)
    {
        printf("Failed to load sound! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }
    return success;
}

bool LSound::loadChunk(const std::string& path)
{
    chunkSound = Mix_LoadWAV(path.c_str());
    bool success = true;
    if (chunkSound == NULL)
    {
        printf("Failed to load sound! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }
    return success;
}

void LSound::playChunk()
{
    Mix_PlayChannel(-1, chunkSound, 0);
}

void LSound::playMusic(int loop)
{
    if (Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(sound, loop);
    }
}