//
// Created by Hector Mejia on 12/11/23.
//

#ifndef RENDER_HEALTH_BAR_SYSTEM_H
#define RENDER_HEALTH_BAR_SYSTEM_H

#include "../asset_store/asset_store.h"
#include "../ecs/ecs.h"
#include "../components/health_component.h"
#include "../components/transform_component.h"
#include "../components/sprite_component.h"

#include <SDL2/SDL.h>

class RenderHeathBarSystem : public System {
public:
    RenderHeathBarSystem() {
        RequireComponent<HealthComponent>();
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
        for (auto entity: GetEntities()) {
            const auto health = entity.GetComponent<HealthComponent>();
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();

            SDL_Color healthBarColor = {255, 255, 255};
            if (health.healthPercentage >= 0 && health.healthPercentage < 40) {
                // 0 - 40 = red
                healthBarColor = {255, 0, 0};
            } else if (health.healthPercentage >= 40 && health.healthPercentage < 80) {
                // 40 - 80 = yellow
                healthBarColor = {255, 255, 0};
            } else if (health.healthPercentage >= 80 && health.healthPercentage <= 100) {
                // 80 - 100 = green
                healthBarColor = {0, 255, 0};
            }

            constexpr int healthBarWidth = 15;
            constexpr int healthBarHeight = 3;
            auto healthBarPosX = transform.position.x + sprite.width * transform.scale.x - camera.x;
            auto healthBarPosY = transform.position.y - camera.y;
            SDL_Rect healthBarPosition = {
                static_cast<int>(healthBarPosX),
                static_cast<int>(healthBarPosY),
                static_cast<int>(healthBarWidth * (health.healthPercentage / 100.0)),
                healthBarHeight
            };

            SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
            SDL_RenderFillRect(renderer, &healthBarPosition);

            std::string healthText = std::to_string(health.healthPercentage);
            SDL_Surface* surface = TTF_RenderText_Blended(
                assetStore->GetFont("pico8-font-5"),
                healthText.c_str(),
                {255, 255, 255}
            );
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWidth = 0;
            int labelHeight = 0;
            SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);
            SDL_Rect healthBarTextRect = {
                static_cast<int>(healthBarPosX),
                static_cast<int>(healthBarPosY) + 5,
                labelWidth,
                labelHeight
            };

            SDL_RenderCopy(renderer, texture, NULL, &healthBarTextRect);
            SDL_DestroyTexture(texture);
        }
    }
};

#endif //RENDER_HEALTH_BAR_SYSTEM_H
