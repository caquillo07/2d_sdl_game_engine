//
// Created by hector on 11/20/23.
//

#ifndef RENDER_COLLIDER_SYSTEM_H
#define RENDER_COLLIDER_SYSTEM_H

#include <SDL2/SDL.h>

#include "../components/box_collider_component.h"
#include "../components/transform_component.h"
#include "../ecs/ecs.h"

class RenderColliderSystem : public System {
public:
    RenderColliderSystem() {
        RequireComponent<BoxColliderComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Renderer* renderer) const {
        for (auto entity: GetEntities()) {
            const auto transformComponent = entity.GetComponent<TransformComponent>();
            const auto colliderComponent = entity.GetComponent<BoxColliderComponent>();

            SDL_Rect colliderRect = {
                static_cast<int>(transformComponent.position.x + colliderComponent.offset.x),
                static_cast<int>(transformComponent.position.y + colliderComponent.offset.y),
                static_cast<int>(colliderComponent.width),
                static_cast<int>(colliderComponent.height),
            };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &colliderRect);
        }
    }
};

#endif //RENDER_COLLIDER_SYSTEM_H
