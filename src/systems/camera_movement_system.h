//
// Created by hector on 11/20/23.
//

#ifndef CAMERA_MOVEMENT_SYSTEM_H
#define CAMERA_MOVEMENT_SYSTEM_H

#include <SDL2/SDL.h>

#include "../game/game.h"
#include "../ecs/ecs.h"
#include "../components/camera_component.h"
#include "../components/transform_component.h"

class CameraMovementSystem : public System {
public:
    CameraMovementSystem() {
        RequireComponent<CameraComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Rect& camera) {
        for (auto entity: GetEntities()) {
            const auto& transform = entity.GetComponent<TransformComponent>();

            if (transform.position.x + (camera.w/2) < Game::mapWidth) {
                camera.x = transform.position.x -(Game::windowWidth/2);
            }
            if (transform.position.y + (camera.h/2) < Game::mapHeight) {
                camera.y = transform.position.y -(Game::windowHeight/2);
            }

            camera.x = camera.x < 0 ? 0 : camera.x;
            camera.y = camera.y < 0 ? 0 : camera.y;
            camera.x = camera.x > Game::mapWidth ? Game::mapWidth : camera.x;
            camera.y = camera.y > Game::mapHeight ? Game::mapHeight : camera.y;
        }
    }
};

#endif //CAMERA_MOVEMENT_SYSTEM_H
