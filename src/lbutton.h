#pragma once
#include "header.h"
#include "ltexture.h"
#include "lsound.h"
#include "lrandom.h"

class LButton
{
public:

    //Sets top left position
    void setPosition(int x, int y, int w, int h);

    //Shows button sprite
    void render(SDL_Renderer* renderer, Texture& x, SDL_Rect* clip);

    bool checkMousein();

    void checkchunkwhenMousein(LSound* sound, LSound* sound1);

    // int getx() { return mPosition.x; }
    // int gety() { return mPosition.y; }
    // int getw() { return mPosition.w; }
    // int geth() { return mPosition.h; }

    // info of this button
    SDL_Rect mPosition;

    //Currently used global sprite
    int buttonState;
    int bridgeleftrightColor, bridgeupdownColor;
    bool mousein = false;

private:
};