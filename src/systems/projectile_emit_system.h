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
#include "../components/camera_component.h"

#include "../logger/logger.h"
#include "../ecs/ecs.h"

class ProjectileEmitSystem : public System {
private:
    void onSpacePressed(KeyPressedEvent& event) {
        if (event.key == SDLK_SPACE) {
            Logger::Log("SPACE PRESSED");
            for (auto entity: GetEntities()) {
                if (entity.HasComponent<CameraComponent>()) {
                    const auto projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                    const auto transform = entity.GetComponent<TransformComponent>();
                    const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

                    // If parent entity has sprite, start the projectile position in the middle of the entity
                    glm::vec2 projectilePosition = transform.position;
                    if (entity.HasComponent<SpriteComponent>()) {
                        const auto sprite = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += (transform.scale.x * sprite.width / 2);
                        projectilePosition.y += (transform.scale.y * sprite.height / 2);
                    }

                    // If parent entity direction is controlled by the keyboard keys, modify the direction of the projectile accordingly
                    glm::vec2 projectileVelocity = projectileEmitter.velocity;
                    int directionX = 0;
                    int directionY = 0;
                    if (rigidbody.velocity.x > 0) directionX = +1;
                    if (rigidbody.velocity.x < 0) directionX = -1;
                    if (rigidbody.velocity.y > 0) directionY = +1;
                    if (rigidbody.velocity.y < 0) directionY = -1;
                    projectileVelocity.x = projectileEmitter.velocity.x * directionX;
                    projectileVelocity.y = projectileEmitter.velocity.y * directionY;

                    // Create new projectile entity and add it to the world
                    Entity projectile = entity.registry->CreateEntity();
                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                    projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
                    projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                    projectile.AddComponent<BoxColliderComponent>(4, 4);
                    projectile.AddComponent<ProjectileComponent>(
                        projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage,
                        projectileEmitter.duration
                    );
                }
            }
        }
    }

public:
    ProjectileEmitSystem() {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }

    void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::onSpacePressed);
    }

    void Update(const std::unique_ptr<Registry>& registry) {
        for (auto entities: GetEntities()) {
            auto& projectileEmitterComponent = entities.GetComponent<ProjectileEmitterComponent>();
            const auto transformComponent = entities.GetComponent<TransformComponent>();

            if (projectileEmitterComponent.frequency == 0) {
                continue;
            }
            
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
