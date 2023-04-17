#include "ltexture.h"

bool Texture::loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Texture* gTexture = NULL;
    gTexture = IMG_LoadTexture(renderer, path);
    if (gTexture == NULL) {
        cerr << "Can't not create texture: " << path << endl;
    }
    else {
        SDL_QueryTexture(gTexture, NULL, NULL, &dest.w, &dest.h);
    }
    texture = gTexture;
    return (texture != NULL);
}

void Texture::drawTexture(SDL_Renderer* renderer) {

    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

void Texture::setDest(int x, int y, int w, int h) {
    dest = { x, y, w, h };
}

void Texture::render(SDL_Renderer* renderer, SDL_Rect* renderQuad, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Render to screen
    SDL_RenderCopyEx(renderer, texture, clip, renderQuad, angle, center, flip);
}