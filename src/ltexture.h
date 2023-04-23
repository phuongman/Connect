#pragma once
#include "header.h"

class Texture {

public:
    SDL_Texture* texture;
    SDL_Rect dest;

    Texture();
    ~Texture();
    void free();
    bool loadTexture(SDL_Renderer* renderer, const char* path);
    void drawTexture(SDL_Renderer* renderer);
    void setDest(int x, int y, int w, int h);
    void render(SDL_Renderer* renderer, SDL_Rect* renderQuad, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    bool loadFromRenderedText(SDL_Renderer* renderer, TTF_Font* font, string textureText, SDL_Color textColor);

private:

};