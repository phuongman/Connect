#pragma once
#include "header.h"

class LSound
{
public:
    Mix_Music* sound = NULL;
    Mix_Chunk* chunkSound = NULL;

    LSound();

    ~LSound();

    void free();

    //Load music from file
    bool loadMusic(const std::string& path);

    //Load chunk from file
    bool loadChunk(const std::string& path);

    void playChunk();

    //Playing music
    void playMusic(int loop);

    //Pause music
    void pauseMusic();

    //Resume music
    void resumeMusic();

    //Stop playing music
    void stopMusic();
};