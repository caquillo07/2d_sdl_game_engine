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

class RenderSystem : public System {
    //: public System {
public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera) const {
        // sort all the entities of our system by their zIndex
        struct RenderableEntity {
            TransformComponent transformComponent;
            SpriteComponent spriteComponent;
        };
        std::vector<RenderableEntity> renderableEntities;
        for (auto entity: GetEntities()) {
            RenderableEntity renderableEntity = {
                .transformComponent = entity.GetComponent<TransformComponent>(),
                .spriteComponent = entity.GetComponent<SpriteComponent>()
            };

            // Check if the entity sprite is outside the camera view
            bool isOutsideCameraView = (
                renderableEntity.transformComponent.position.x + (renderableEntity.transformComponent.scale.x * renderableEntity.spriteComponent.width) < camera.x ||
                renderableEntity.transformComponent.position.x > camera.x + camera.w ||
                renderableEntity.transformComponent.position.y + (renderableEntity.transformComponent.scale.y * renderableEntity.spriteComponent.height) < camera.y ||
                renderableEntity.transformComponent.position.y > camera.y + camera.h
            );

            // Cull sprites that are outside the camera view (and are not fixed)
            if (isOutsideCameraView && !renderableEntity.spriteComponent.isFixed) {
                continue;
            }

            renderableEntities.emplace_back(renderableEntity);
        }
        std::sort(
            renderableEntities.begin(),
            renderableEntities.end(),
            [](const RenderableEntity& a, const RenderableEntity& b) {
                return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
            }
        );

        for (const auto& entity: renderableEntities) {
            const auto spriteComponent = entity.spriteComponent;
            const auto transformComponent = entity.transformComponent;

            // set the source rect for the sprite texture

            SDL_Rect destinationRect = {
                .x = static_cast<int>(transformComponent.position.x - (spriteComponent.isFixed ? 0 : camera.x)),
                .y = static_cast<int>(transformComponent.position.y - (spriteComponent.isFixed ? 0 : camera.y)),
                .w = static_cast<int>(spriteComponent.width * transformComponent.scale.x),
                .h = static_cast<int>(spriteComponent.height * transformComponent.scale.y)
            };

            SDL_RenderCopyEx(
                renderer,
                assetStore->GetTexture(spriteComponent.textureAssetID),
                &spriteComponent.srcRect,
                &destinationRect,
                transformComponent.rotation,
                nullptr,
                spriteComponent.flip
            );
        }
    }
};

#endif //INC_2D_SDL_GAME_ENGINE_RENDER_SYSTEM_H
