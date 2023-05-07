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

struct dataSaveTopScore {
    string name;
    int score;
    bool operator < (const dataSaveTopScore& x) const {
        return score > x.score;
    }
};

class Game {

public:

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    TTF_Font* playerNameFont = NULL;
    TTF_Font* scoreFont = NULL;
    TTF_Font* topScoreFont = NULL;
    TTF_Font* gameOverFont = NULL;
    TTF_Font* roundFont = NULL;

    Game(SDL_Window* gWindow, SDL_Renderer* gRenderer);
    ~Game();
    bool loadMedia();

    void renderScoreInGame();
    void topScoreRender();
    bool readTopScore();
    void printTopScore();
    bool readPlayerName();
    void renameRender();
    void updateName(Texture& name, Texture& name1);
    void printPlayerName();
    void updateTopScore();

    void stopMusic();
    void continueMusic();
    void resetMusic();
    void playingMusic();

    void RollBackOneButton();
    void RollBack();
    void Successful();
    LButton* getSecondbutton();
    void updateButton(LButton* curButton, int updateState);
    int getPosstate(int x, int y, int xx, int yy);
    void getNextstate(LButton* prevButton, LButton* curButton, bool goal);
    bool checkMouseouttable();
    bool checkBridge(LButton* curButton);
    void pauseGamePls();
    void resetTimeliquid();
    void movetimeliquidLeft();
    void timeDecay(double& timeperTile, double& curTime);

    bool loadLevel(const char* path);
    void gameStart();
    void playGame();
    void gameLoop();

    void free();
private:

};