//
// Created by hector on 11/6/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_RENDER_SYSTEM_H
#define INC_2D_SDL_GAME_ENGINE_RENDER_SYSTEM_H

#include <SDL2/SDL_render.h>

#include <algorithm>

#include "../asset_store/asset_store.h"
#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/sprite_component.h"
#include "../logger/logger.h"

class RenderSystem : public System { //: public System {
public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
        // sort all the entities of our system by their zIndex
        struct RenderableEntity {
            TransformComponent transformComponent;
            SpriteComponent spriteComponent;
        };
        std::vector<RenderableEntity> renderableEntities;
        for (auto entity : GetEntities()) {
            RenderableEntity renderableEntity = {
                    .transformComponent = entity.GetComponent<TransformComponent>(),
                    .spriteComponent = entity.GetComponent<SpriteComponent>()
            };
            renderableEntities.emplace_back(renderableEntity);
        }
        std::sort(
                renderableEntities.begin(),
                renderableEntities.end(),
                [](const RenderableEntity& a, const RenderableEntity& b) {
                    return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
                }
        );
        
        for (auto entity: renderableEntities) {
            const auto spriteComponent = entity.spriteComponent;
            const auto transformComponent = entity.transformComponent;

            // set the source rect for the sprite texture

            SDL_Rect destinationRect = {
                    .x = static_cast<int>(transformComponent.position.x),
                    .y= static_cast<int>(transformComponent.position.y),
                    .w =static_cast<int>(spriteComponent.width * transformComponent.scale.x),
                    .h = static_cast<int>(spriteComponent.height * transformComponent.scale.y)
            };

            SDL_RenderCopyEx(
                    renderer,
                    assetStore->GetTexture(spriteComponent.textureAssetID),
                    &spriteComponent.srcRect,
                    &destinationRect,
                    transformComponent.rotation,
                    nullptr,
                    SDL_FLIP_NONE
            );
        }
    }
};

#endif //INC_2D_SDL_GAME_ENGINE_RENDER_SYSTEM_H
