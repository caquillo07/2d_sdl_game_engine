#include "game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include <charconv>
#include <cstdio>
#include <iostream>
#include <ostream>

Game::Game()
    : isRunning(false), millisecondsPreviousFrame(0), windowWidth(0),
      windowHeight(0) {
    std::cout << "Game constructor" << std::endl;
}

Game::~Game() {
    isRunning = false;
    std::cout << "Game destructor" << std::endl;
}

void Game::Run() {
    Setup();
    while (this->isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::Setup() {
    playerPosition = glm::vec2(10.0f, 20.0f);
    playerVelocity = glm::vec2(25.0f, 0.0f);
}

void Game::Update() {
    // Wait until 16ms has elapsed since last frame
    int timeToWait =
        MILLIS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLIS_PER_FRAME) {
        SDL_Delay(timeToWait);
    }

    double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0f;

    millisecondsPreviousFrame = SDL_GetTicks();

    playerPosition.x += playerVelocity.x * deltaTime;
    playerPosition.y += playerVelocity.y * deltaTime;

    // print FPS
    std::cout << "FPS: " << 1.0f / deltaTime << std::endl;
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "FPS: %d", (int)ceil(1.0f / deltaTime));
    SDL_SetWindowTitle(this->window, buffer);
}

void Game::Render() {
    SDL_SetRenderDrawColor(this->renderer, 21, 21, 21, 255);
    SDL_RenderClear(this->renderer);

    // Draw png texture
    SDL_Surface *surface = IMG_Load("./assets/images/tank-tiger-right.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect destRect = {static_cast<int>(playerPosition.x),
                         static_cast<int>(playerPosition.y), 32, 32};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(this->renderer);
}

void Game::Destroy() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error init SDL" << std::endl;
        return;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        std::cerr << "failed to get current display mode: " << SDL_GetError()
                  << "\n"
                  << std::endl;
        return;
    }
    // this->windowWidth = displayMode.w;
    // this->windowHeight = displayMode.h;
    this->windowWidth = 800;  // displayMode.w;
    this->windowHeight = 600; // displayMode.h;

    this->window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, this->windowWidth,
                                    this->windowHeight, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Error creating SDL window" << std::endl;
        return;
    }

    // -1 means give default
    // SDL2 should know to use dedicated GPU by default, but in case we wanted
    // to be explicit, this is how.
    this->renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Error creating SDL renderer" << std::endl;
        return;
    }

    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    this->isRunning = true;
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
        case SDL_QUIT:
            this->isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                this->isRunning = false;
            }
            break;
        }
    }
}
