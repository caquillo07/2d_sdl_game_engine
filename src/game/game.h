#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <sol/sol.hpp>

#include "../ecs/ecs.h"
#include "../asset_store/asset_store.h"
#include "../event_bus/event_bus.h"

constexpr int FPS              = 120;
constexpr int MILLIS_PER_FRAME = 1000 / FPS;

class Game {
    private:
        bool isRunning;
        bool isDebug;
        bool isFreezed;
        int millisecondsPreviousFrame;

        SDL_Rect camera;
        SDL_Window* window;
        SDL_Renderer* renderer;

        sol::state lua;

        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetStore> assetStore;
        std::unique_ptr<EventBus> eventBus;

    public:
        static int windowWidth;
        static int windowHeight;
        static int mapWidth;
        static int mapHeight;

        Game();
        ~Game();

        void Initialize();
        void Run();
        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy() const;
};

#endif // !GAME_H
