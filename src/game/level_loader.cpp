//
// Created by Hector Mejia on 12/21/23.
//

#include <fstream>


#include "level_loader.h"
#include "game.h"
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

LevelLoader::LevelLoader() {
}

LevelLoader::~LevelLoader() {
}

void LevelLoader::LoadLevel(
    const std::unique_ptr<Registry>& registry,
    const std::unique_ptr<AssetStore>& assetStore,
    SDL_Renderer* renderer,
    int i
) {
    // adding assets to asset store
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
    assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");
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
    Game::mapHeight = mapNumRows * tileSize * tileScale;
    Game::mapWidth = mapNumCols * tileSize * tileScale;
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
    radar.AddComponent<TransformComponent>(glm::vec2(Game::windowWidth - 64 * 2, 10.0), glm::vec2(2.f, 2.f), 0.f);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.f, 0.f));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);


    Entity tank = registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(500.f, 500.f), glm::vec2(1.f, 1.f), 0.f);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(100.f, 0.f));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    // tank.AddComponent<ProjectileEmitterComponent>(
    //     glm::vec2(-100.f, 0.f),
    //     5000,
    //     3000,
    //     10,
    //     false
    // );
    tank.AddComponent<HealthComponent>(100);

    Entity treeA = registry->CreateEntity();
    treeA.Group("obstacles");
    treeA.AddComponent<TransformComponent>(glm::vec2(600.f, 495.f), glm::vec2(1.f, 1.f), 0.f);
    treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
    treeA.AddComponent<BoxColliderComponent>(16, 32, glm::vec2(0));

    Entity treeB = registry->CreateEntity();
    treeB.Group("obstacles");
    treeB.AddComponent<TransformComponent>(glm::vec2(400.f, 495.f), glm::vec2(1.f, 1.f), 0.f);
    treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
    treeB.AddComponent<BoxColliderComponent>(16, 32, glm::vec2(0));

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
        glm::vec2(Game::windowWidth / 2 - 40, 10),
        "CHOPPER 1.0",
        "charriot-font",
        white,
        true
    );
}
