#pragma once
#include "header.h"
#include "ltexture.h"
#include "lrandom.h"
#include "lbutton.h"
#include "lsound.h"

struct dataRollback {
    LButton* saveButton;
    int oldbuttonState;
};

class Game {

public:

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    TTF_Font* font = NULL;

    Game(SDL_Window* gWindow, SDL_Renderer* gRenderer);
    ~Game();
    void RollBackOneButton();
    void RollBack();
    void Successful();
    LButton* getSecondbutton();
    void updateButton(LButton* curButton, int updateState);
    int getPosstate(int x, int y, int xx, int yy);
    void getNextstate(LButton* prevButton, LButton* curButton, bool goal);
    bool checkMouseouttable();
    bool checkBridge(LButton* curButton);

    void stopMusic();
    void continueMusic();
    void resetMusic();
    void playingMusic();

    void pauseGamePls();

    void resetTimeliquid();
    void movetimeliquidLeft();
    void timeDecay(double& timeperTile, double& curTime);
    bool loadLevel(const char* path);
    bool loadMedia();
    void playGame();
    void gameLoop();
    void gameStart();

private:

};