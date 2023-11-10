//
// Created by hector on 11/7/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_ANIMATION_SYSTEM_H
#define INC_2D_SDL_GAME_ENGINE_ANIMATION_SYSTEM_H

#include <SDL2/SDL.h>

#include "../ecs/ecs.h"
#include "../components/animation_component.h"
#include "../components/sprite_component.h"

class AnimationSystem : public System {
public:
    AnimationSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<AnimationComponent>();
    }

    void Update() const {
        for (auto entity: GetEntities()) {
            auto& animationComponent = entity.GetComponent<AnimationComponent>();
            auto& spriteComponent = entity.GetComponent<SpriteComponent>();

            animationComponent.currentFrame =
                    ((static_cast<int>(SDL_GetTicks()) - animationComponent.startTime) * animationComponent.
                     frameRateSpeed / 1000) %
                    animationComponent.numFrames;
            spriteComponent.srcRect.x = spriteComponent.width * animationComponent.currentFrame;
        }
    }
};

#endif //INC_2D_SDL_GAME_ENGINE_ANIMATION_SYSTEM_H
