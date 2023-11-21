//
// Created by hector on 11/20/23.
//

#ifndef PROJECTILE_EMIT_SYSTEM_H
#define PROJECTILE_EMIT_SYSTEM_H

#include <SDL2/SDL.h>

#include "../components/projectile_component.h"
#include "../components/projectile_emitter_component.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../components/sprite_component.h"
#include "../components/box_collider_component.h"

#include "../ecs/ecs.h"

class ProjectileEmitSystem : public System {
public:
    ProjectileEmitSystem() {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(const std::unique_ptr<Registry>& registry) {
        for (auto entities: GetEntities()) {
            auto& projectileEmitterComponent = entities.GetComponent<ProjectileEmitterComponent>();
            const auto transformComponent = entities.GetComponent<TransformComponent>();

            if (static_cast<int>(SDL_GetTicks()) - projectileEmitterComponent.lastEmissionTime >
                projectileEmitterComponent.frequency) {
                glm::vec2 projectilePosition = transformComponent.position;
                if (entities.HasComponent<SpriteComponent>()) {
                    const auto spriteComponent = entities.GetComponent<SpriteComponent>();
                    projectilePosition.x += (transformComponent.scale.x * spriteComponent.width / 2);
                    projectilePosition.y += (transformComponent.scale.y * spriteComponent.height / 2);
                }

                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<TransformComponent>(
                    projectilePosition,
                    glm::vec2(1.f, 1.f),
                    0
                );

                projectile.AddComponent<RigidBodyComponent>(projectileEmitterComponent.velocity);
                projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                projectile.AddComponent<BoxColliderComponent>(4, 4);
                projectile.AddComponent<ProjectileComponent>(
                    projectileEmitterComponent.isFriendly,
                    projectileEmitterComponent.hitPercentDamage,
                    projectileEmitterComponent.duration
                );
                projectileEmitterComponent.lastEmissionTime = static_cast<int>(SDL_GetTicks());
            }
        }
    }
};

#endif //PROJECTILE_EMIT_SYSTEM_H
