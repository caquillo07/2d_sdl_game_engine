#include <cstdio>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_sdl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <sol/sol.hpp>

#include "level_loader.h"
#include "game.h"
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
#include "../systems/render_gui_system.h"
#include "../systems/render_health_bar_system.h"
#include "../systems/render_system.h"
#include "../systems/render_text_system.h"
#include "../systems/script_system.h"


int Game::mapWidth     = 0;
int Game::mapHeight    = 0;
int Game::windowWidth  = 0;
int Game::windowHeight = 0;

Game::Game() : isRunning(false),
               isDebug(false),
               isFreezed(false),
               millisecondsPreviousFrame(0),
               camera(SDL_Rect{}),
               window(nullptr),
               renderer(nullptr) {
    this->registry   = std::make_unique<Registry>();
    this->assetStore = std::make_unique<AssetStore>();
    this->eventBus   = std::make_unique<EventBus>();

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

void Game::Setup() {
    // add a system that needs to be processed in our game
    this->registry->AddSystem<DamageSystem>();
    this->registry->AddSystem<RenderSystem>();
    this->registry->AddSystem<MovementSystem>();
    this->registry->AddSystem<AnimationSystem>();
    this->registry->AddSystem<RenderGUISystem>();
    this->registry->AddSystem<RenderTextSystem>();
    this->registry->AddSystem<BoxColliderSystem>();
    this->registry->AddSystem<RenderColliderSystem>();
    this->registry->AddSystem<KeyboardControlSystem>();
    this->registry->AddSystem<CameraMovementSystem>();
    this->registry->AddSystem<ProjectileEmitSystem>();
    this->registry->AddSystem<RenderHeathBarSystem>();
    this->registry->AddSystem<ProjectileLifecycleSystem>();
    this->registry->AddSystem<ScriptSystem>();

    this->registry->GetSystem<ScriptSystem>().CreateLuaBindings(lua);

    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
    LevelLoader::LoadLevel(lua, registry, assetStore, renderer, 2);
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
    registry->GetSystem<MovementSystem>().SubscribeToEvents(this->eventBus);

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
    registry->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());

    // *************************************************************************
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
    windowWidth  = 1280; // displayMode.w;
    windowHeight = 720;  // displayMode.h;
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
        io.MousePos       = ImVec2(mouseX, mouseY);
        io.MouseDown[0]   = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1]   = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

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
