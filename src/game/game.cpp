#include <cstdio>
#include <fstream>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>

#include "./game.h"
#include "../components/animation_component.h"
#include "../components/box_collider_component.h"
#include "../components/camera_component.h"
#include "../components/health_component.h"
#include "../components/keyword_controlled_component.h"
#include "../components/projectile_emitter_component.h"
#include "../components/rigid_body_component.h"
#include "../components/sprite_component.h"
#include "../components/text_label_component.h"
#include "../components/transform_component.h"
#include "../ecs/ecs.h"
#include "../systems/animation_system.h"
#include "../systems/box_collider_system.h"
#include "../systems/camera_movement_system.h"
#include "../systems/damage_system.h"
#include "../systems/keyboard_control_system.h"
#include "../systems/movement_system.h"
#include "../systems/projectile_emit_system.h"
#include "../systems/projectile_lifecycle_system.h"
#include "../systems/render_collider_system.h"
#include "../systems/render_system.h"
#include "../systems/render_text_system.h"
#include "../systems/render_health_bar_system.h"
#include "../systems/render_gui_system.h"


int Game::windowWidth = 0;
int Game::windowHeight = 0;
int Game::mapWidth = 0;
int Game::mapHeight = 0;

Game::Game() : isRunning(false),
               isDebug(false),
               isFreezed(false),
               millisecondsPreviousFrame(0),
               camera(SDL_Rect{}),
               window(nullptr),
               renderer(nullptr) {
    this->registry = std::make_unique<Registry>();
    this->assetStore = std::make_unique<AssetStore>();
    this->eventBus = std::make_unique<EventBus>();

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

void Game::LoadLevel(int levelNumber) const {
    // add a system that needs to be processed in our game
    this->registry->AddSystem<MovementSystem>();
    this->registry->AddSystem<RenderSystem>();
    this->registry->AddSystem<AnimationSystem>();
    this->registry->AddSystem<BoxColliderSystem>();
    this->registry->AddSystem<RenderColliderSystem>();
    this->registry->AddSystem<DamageSystem>();
    this->registry->AddSystem<KeyboardControlSystem>();
    this->registry->AddSystem<CameraMovementSystem>();
    this->registry->AddSystem<ProjectileEmitSystem>();
    this->registry->AddSystem<ProjectileLifecycleSystem>();
    this->registry->AddSystem<RenderTextSystem>();
    this->registry->AddSystem<RenderHeathBarSystem>();
    this->registry->AddSystem<RenderGUISystem>();

    // adding assets to asset store
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
    assetStore->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 20);
    assetStore->AddFont("pico8-font-5", "./assets/fonts/pico8.ttf", 5);
    assetStore->AddFont("pico8-font-10", "./assets/fonts/pico8.ttf", 5);

    // Load the tilemap
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

    // ugly, and does not check errors, but it works for now
    // Load the tilemap

    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");
    std::cout << mapFile.is_open();

    int tileSize = 32;
    constexpr int mapNumRows = 20;
    constexpr int mapNumCols = 25;
    constexpr double tileScale = 2.0;
    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.Group("tiles");
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
                false,
                srcRectX,
                srcRectY
            );
        }
    }
    mapFile.close();
    mapHeight = mapNumRows * tileSize * tileScale;
    mapWidth = mapNumCols * tileSize * tileScale;
    constexpr double MOVE_SPEED = 100.f;

    Entity chopper = registry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(245.f, 120.f), glm::vec2(1.f, 1.f), 0.f);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.f, 0.f));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    chopper.AddComponent<KeywordControlledComponent>(
        glm::vec2(0.f, -MOVE_SPEED),
        glm::vec2(MOVE_SPEED, 0.f),
        glm::vec2(0.f, MOVE_SPEED),
        glm::vec2(-MOVE_SPEED, 0.f)
    );
    chopper.AddComponent<CameraComponent>();
    chopper.AddComponent<HealthComponent>(100);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 25, true);


    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 64 * 2, 10.0), glm::vec2(2.f, 2.f), 0.f);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.f, 0.f));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);


    Entity tank = registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(470.f, 400.f), glm::vec2(1.f, 1.f), 0.f);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(0.f, 0.f));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    tank.AddComponent<ProjectileEmitterComponent>(
        glm::vec2(-100.f, 0.f),
        5000,
        3000,
        10,
        false
    );
    tank.AddComponent<HealthComponent>(100);


    Entity truck = registry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(200.f, 500.f), glm::vec2(1.f, 1.f), 0.f);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.f, 0.f));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    truck.AddComponent<ProjectileEmitterComponent>(
        glm::vec2(0.f, -100.f),
        2000,
        5000,
        10,
        false
    );
    truck.AddComponent<HealthComponent>(100);

    Entity label = registry->CreateEntity();
    SDL_Color white = {255, 255, 255};
    label.AddComponent<TextLabelComponent>(
        glm::vec2(windowWidth / 2 - 40, 10),
        "CHOPPER 1.0",
        "charriot-font",
        white,
        true
    );
}

void Game::Setup() const {
    LoadLevel(1);
}

void Game::Update() {
    // Wait until 16ms has elapsed since last frame
    const int timeToWait =
            MILLIS_PER_FRAME - (static_cast<int>(SDL_GetTicks()) - millisecondsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLIS_PER_FRAME) {
        SDL_Delay(timeToWait);
    }

    const float deltaTime = (static_cast<float>(SDL_GetTicks()) - static_cast<float>(millisecondsPreviousFrame)) /
                            1000.0f;

    millisecondsPreviousFrame = static_cast<int>(SDL_GetTicks());

    // reset all event handlers for current frame
    this->eventBus->Reset();

    // perform the subscription of the events
    registry->GetSystem<DamageSystem>().SubscribeToEvents(this->eventBus);
    registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(this->eventBus);
    registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(this->eventBus);

    // update the registry to process the entities that are waiting to be created/destroyed
    registry->Update();

    // Ask all systems to run
    if (!this->isFreezed) {
        registry->GetSystem<MovementSystem>().Update(deltaTime);
    }
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<BoxColliderSystem>().Update(this->eventBus);
    registry->GetSystem<ProjectileEmitSystem>().Update(this->registry);
    registry->GetSystem<CameraMovementSystem>().Update(this->camera);
    registry->GetSystem<ProjectileLifecycleSystem>().Update();

    // **************************************************************************
    // print FPS
    //    Logger::Log("FPS: " + std::to_string(1.0f / deltaTime));
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "FPS: %d", static_cast<int>(ceil(1.0f / deltaTime)));
    SDL_SetWindowTitle(this->window, buffer);
}

void Game::Render() {
    SDL_SetRenderDrawColor(this->renderer, 21, 21, 21, 255);
    SDL_RenderClear(this->renderer);

    // invoke all systems that need to render
    registry->GetSystem<RenderSystem>().Update(this->renderer, this->assetStore, this->camera);
    registry->GetSystem<RenderTextSystem>().Update(
        this->renderer,
        this->assetStore,
        this->camera
    );
    registry->GetSystem<RenderHeathBarSystem>().Update(
        this->renderer,
        this->assetStore,
        this->camera
    );
    if (this->isDebug) {
        registry->GetSystem<RenderColliderSystem>().Update(this->renderer, this->camera);
        registry->GetSystem<RenderGUISystem>().Update(registry, camera);
    }

    SDL_RenderPresent(this->renderer);
}

void Game::Destroy() const {
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error init SDL");
        return;
    }

    if (TTF_Init() != 0) {
        Logger::Err("Error init SDL TTF");
        return;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        Logger::Err(
            "failed to get current display mode: " +
            std::string(SDL_GetError())
        );
        return;
    }
    // this->windowWidth = displayMode.w;
    // this->windowHeight = displayMode.h;
    windowWidth = 800; // displayMode.w;
    windowHeight = 600; // displayMode.h;
    // this->windowWidth = 25 * 32 * 2; // displayMode.w;
    // this->windowHeight = 20 * 32 * 2; // displayMode.h;

    this->window = SDL_CreateWindow(
        nullptr, SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, windowWidth,
        windowHeight, SDL_WINDOW_RESIZABLE
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

    // Initialize the ImGui context
    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);


    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    this->camera.x = 0;
    this->camera.y = 0;
    this->camera.w = windowWidth;
    this->camera.h = windowHeight;

    this->isRunning = true;
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        // ImGui SDL Input
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO& io = ImGui::GetIO();
        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        switch (sdlEvent.type) {
            case SDL_QUIT: this->isRunning = false;
                break;
            case SDL_KEYDOWN:
                this->eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    this->isRunning = false;
                }
                if (sdlEvent.key.keysym.sym == SDLK_F1) {
                    this->isDebug = !this->isDebug;
                }
                if (sdlEvent.key.keysym.sym == SDLK_f) {
                    this->isFreezed = !this->isFreezed;
                }
                break;
            default: ;
        }
    }
}
