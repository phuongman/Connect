#pragma once
#include "header.h"
#include "ltexture.h"

class LButton
{
public:

    //Sets top left position
    void setPosition(int x, int y, int w, int h);

    //Handles mouse event
    void handleEvent(SDL_Event* e);

    //Shows button sprite
    void render(SDL_Renderer* renderer, Texture& x, SDL_Rect* clip);

    bool checkMousein();

    // int getx() { return mPosition.x; }
    // int gety() { return mPosition.y; }
    // int getw() { return mPosition.w; }
    // int geth() { return mPosition.h; }

    // info of this button
    SDL_Rect mPosition;

    //Currently used global sprite
    int buttonState;

private:
};