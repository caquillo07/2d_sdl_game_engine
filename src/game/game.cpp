#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include <charconv>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <ostream>

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../components/sprite_component.h"
#include "../components/animation_component.h"
#include "../systems/animation_system.h"
#include "../systems/movement_system.h"
#include "../systems/render_system.h"
#include "../logger/logger.h"
#include "./game.h"

Game::Game() :
        isRunning(false),
        millisecondsPreviousFrame(0),
        windowWidth(0),
        windowHeight(0) {

    this->registry = std::make_unique<Registry>();
    this->assetStore = std::make_unique<AssetStore>();
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

void Game::LoadLevel(int levelNumber) {
    // todo(hector) - implement

    // add a system that needs to be processed in our game
    this->registry->AddSystem<MovementSystem>();
    this->registry->AddSystem<RenderSystem>();
    this->registry->AddSystem<AnimationSystem>();

    // adding assets to asset store
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");

    // Load the tilemap
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

    // ugly, and does not check errors, but it works for now
    // Load the tilemap
    int tileSize = 32;
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;

    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");
    std::cout << mapFile.is_open();

    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(
                    glm::vec2(
                            x * (tileScale * tileSize),
                            y * (tileScale * tileSize)
                    ),
                    glm::vec2(tileScale, tileScale),
                    0.0
            );
            tile.AddComponent<SpriteComponent>(
                    "tilemap-image",
                    tileSize,
                    tileSize,
                    0,
                    srcRectX,
                    srcRectY
            );
        }
    }
    mapFile.close();

    Entity chopper = registry->CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(10.f, 10.f), glm::vec2(2.f, 2.f), 0.f);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.f, 0.f));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    
    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 64*2, 10.0), glm::vec2(2.f, 2.f), 0.f);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.f, 0.f));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10.f, 10.f), glm::vec2(2.f, 2.f), 45.f);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(50.f, 0.f));
    tank.AddComponent<SpriteComponent>("tank-image",  32, 32, 2);

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(10.f, 30.f), glm::vec2(2.f, 2.f), 0.f);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(20.f, 50.f));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
}
void Game::Setup() {
    LoadLevel(1);
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

    // Ask all systems to run
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update();

    // update the registry to process the entities that are waiting to be created/destroyed
    registry->Update();


    // **************************************************************************
    // print FPS
//    Logger::Log("FPS: " + std::to_string(1.0f / deltaTime));
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "FPS: %d", (int) ceil(1.0f / deltaTime));
    SDL_SetWindowTitle(this->window, buffer);
}

void Game::Render() {
    SDL_SetRenderDrawColor(this->renderer, 21, 21, 21, 255);
    SDL_RenderClear(this->renderer);

    // invoke all systems that need to render
    registry->GetSystem<RenderSystem>().Update(this->renderer, this->assetStore);

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
        Logger::Err(
                "failed to get current display mode: " +
                std::string(SDL_GetError()));
        return;
    }
    // this->windowWidth = displayMode.w;
    // this->windowHeight = displayMode.h;
//    this->windowWidth = 800;  // displayMode.w;
//    this->windowHeight = 600; // displayMode.h;
    this->windowWidth = 25 * 32 * 2;  // displayMode.w;
    this->windowHeight = 20 * 32 * 2; // displayMode.h;

    this->window = SDL_CreateWindow(
            NULL, SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, this->windowWidth,
            this->windowHeight, SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        Logger::Err("Error creating SDL window");
        return;
    }

    // -1 means give default
    // SDL2 should know to use dedicated GPU by default, but in case we wanted
    // to be explicit, this is how.
    this->renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
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
            case SDL_QUIT:this->isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    this->isRunning = false;
                }
                break;
        }
    }
}
