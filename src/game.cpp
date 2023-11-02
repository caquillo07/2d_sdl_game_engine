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

#include "game.h"
#include "logger.h"

Game::Game()
    : isRunning(false), millisecondsPreviousFrame(0), windowWidth(0),
      windowHeight(0) {
    Logger::Log("Game constructor");
}

Game::~Game() {
    isRunning = false;
    Logger::Log("Game destructor");
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
    Logger::Log("FPS: " + std::to_string(1.0f / deltaTime));
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
        Logger::Err("Error init SDL");
        return;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        Logger::Err("failed to get current display mode: " +
                    std::string(SDL_GetError()));
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
        Logger::Err("Error creating SDL window");
        return;
    }

    // -1 means give default
    // SDL2 should know to use dedicated GPU by default, but in case we wanted
    // to be explicit, this is how.
    this->renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        Logger::Err("Error creating SDL renderer");
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
