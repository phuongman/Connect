#include "ltexture.h"

Texture::Texture() {
    texture = NULL;
    dest = { 0, 0, 0, 0 };
}

Texture::~Texture() {
    free();
}

void Texture::free() {
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
}

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


/*
    renderQuad là vị trí của cái button trên màn hình
    clip là tọa độ hcn để lấy ảnh
*/
void Texture::render(SDL_Renderer* renderer, SDL_Rect* renderQuad, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Render to screen
    SDL_RenderCopyEx(renderer, texture, clip, renderQuad, angle, center, flip);
}

bool Texture::loadFromRenderedText(SDL_Renderer* renderer, TTF_Font* font, string textureText, SDL_Color textColor)
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, textureText.c_str(), textColor);
    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        //Create texture from surface pixels
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (texture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get image dimensions
            dest.w = textSurface->w;
            dest.h = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    }

    //Return success
    return texture != NULL;
}
