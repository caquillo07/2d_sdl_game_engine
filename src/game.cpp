#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <ostream>

Game::Game() : isRunning(false) {
  std::cout << "Game constructor" << std::endl;
}

Game::~Game() {
  isRunning = false;
  std::cout << "Game destructor" << std::endl;
}

void Game::Run() {
  while (this->isRunning) {
    ProcessInput();
    Update();
    Render();
  }
}

void Game::Render() {}

void Game::Update() {}

void Game::Destroy() {
  SDL_DestroyRenderer(this->renderer);
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}

void Game::Initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING != 0)) {
    std::cerr << "Error init SDL" << std::endl;
    return;
  }

  this->window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       800, 600, SDL_WINDOW_BORDERLESS);
  if (!window) {
    std::cerr << "Error creating SDL window" << std::endl;
    return;
  }

  // -1 means give default
  this->renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    std::cerr << "Error creating SDL renderer" << std::endl;
    return;
  }

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
