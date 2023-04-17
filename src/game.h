#pragma once
#include "header.h"
#include "ltexture.h"
#include "lrandom.h"
#include "lbutton.h"

struct dataRollback {
    LButton* saveButton;
    int oldbuttonState;
};

class Game {

public:

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    Game(SDL_Window* gWindow, SDL_Renderer* gRenderer);
    ~Game();
    bool loadMedia();
    void gameLoop();
    void playGame();
    void RollBackOneButton();
    void RollBack();
    bool checkMouseouttable();
    void Successful();
    LButton* getSecondbutton();
    void updateButton(LButton* curButton, int updateState);
    int getPosstate(int x, int y, int xx, int yy);
    void getNextstate(LButton* prevButton, LButton* curButton, bool goal);
    void gameStart();
    bool loadLevel(const char* path);

private:

};