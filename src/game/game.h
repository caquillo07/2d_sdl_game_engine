#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include "../ecs/ecs.h"
#include "../asset_store/asset_store.h"

constexpr int FPS = 60;
constexpr int MILLIS_PER_FRAME = 1000 / FPS;

class Game {
private:
    bool isRunning;
    int millisecondsPreviousFrame;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;

public:
    Game();
    ~Game();

    void Initialize();
    void Run();
    void LoadLevel(int levelNumber) const;
    void Setup() const;
    void ProcessInput();
    void Update();
    void Render();
    void Destroy() const;

    int windowWidth;
    int windowHeight;
};

#endif // !GAME_H
