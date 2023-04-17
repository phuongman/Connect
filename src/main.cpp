#pragma once
#include "header.h"
#include <stdio.h>
#include "game.h"
const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 1000;

/*
    window that displays
*/
SDL_Window* gWindow;

/*
    renderer of the window
*/
SDL_Renderer* gRenderer;

bool init() {
    bool success = true;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            printf("Warning: Linear texture filtering not enabled!");
        }
        gWindow = SDL_CreateWindow("Connect!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else {
                SDL_SetRenderDrawColor(gRenderer, 125, 219, 251, 255);
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

void close() {
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) {
        printf("failed to initialize SDL!!!!\nSDL Error: %s\n", SDL_GetError());
    }
    else {
        Game game = Game(gWindow, gRenderer);
        game.gameLoop();
    }
    close();
    return 0;
}