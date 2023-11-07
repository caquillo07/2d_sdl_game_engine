#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <glm/glm.hpp>
#include <map>

#include "../ecs/ecs.h"
#include "../asset_store/asset_store.h"

const int FPS = 60;
const int MILLIS_PER_FRAME = 1000 / FPS;

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
    void LoadLevel(int levelNumber);
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();

    int windowWidth;
    int windowHeight;
};

#endif // !GAME_H
