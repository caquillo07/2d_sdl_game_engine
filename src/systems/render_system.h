//
// Created by hector on 11/6/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_RENDER_SYSTEM_H
#define INC_2D_SDL_GAME_ENGINE_RENDER_SYSTEM_H

#include <SDL2/SDL_render.h>

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/sprite_component.h"
#include "../logger/logger.h"

class RenderSystem: public System { //: public System {
public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer* renderer) {
        for (auto entity : GetEntities()) {
            const auto& transformComponent = entity.GetComponent<TransformComponent>();
            const auto& spriteComponent = entity.GetComponent<SpriteComponent>();

            SDL_Rect destinationRectangle = {
                    static_cast<int>(transformComponent.position.x),
                    static_cast<int>(transformComponent.position.y),
                    spriteComponent.width,
                    spriteComponent.height
            };

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &destinationRectangle);
        }
    }
};

#endif //INC_2D_SDL_GAME_ENGINE_RENDER_SYSTEM_H
