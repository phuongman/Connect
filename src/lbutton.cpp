#include "lbutton.h"

void LButton::setPosition(int x, int y, int w, int h)
{
    mPosition = { x, y, w, h };
}

void LButton::render(SDL_Renderer* renderer, Texture& x, SDL_Rect* clip)
{
    //Show current button sprite
    x.render(renderer, &mPosition, clip);
}

bool LButton::checkMousein() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    bool inside = true;
    if (x < mPosition.x) inside = false;
    if (y < mPosition.y) inside = false;
    if (x >= mPosition.x + mPosition.w) inside = false;
    if (y >= mPosition.y + mPosition.h) inside = false;
    return inside;
}
