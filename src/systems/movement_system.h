#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../event_bus/event_bus.h"
#include "../events/collision_event.h"
#include "../logger/logger.h"

class MovementSystem : public System {
    //: public System {
public:
    MovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void Update(const float deltaTime) const {
        for (auto entity: GetEntities()) {
            auto& transformComponent = entity.GetComponent<TransformComponent>();
            const auto& rigidBodyComponent = entity.GetComponent<RigidBodyComponent>();

            transformComponent.position += rigidBodyComponent.velocity * deltaTime;
            // Prevent the main player from moving outside the map boundaries
            if (entity.HasTag("player")) {
                constexpr int paddingLeft = 10;
                constexpr int paddingTop = 10;
                constexpr int paddingRight = 50;
                constexpr int paddingBottom = 50;
                transformComponent.position.x = transformComponent.position.x < paddingLeft
                                                    ? paddingLeft
                                                    : transformComponent.position.x;
                transformComponent.position.x = transformComponent.position.x > Game::mapWidth - paddingRight
                                                    ? Game::mapWidth - paddingRight
                                                    : transformComponent.position.x;
                transformComponent.position.y = transformComponent.position.y < paddingTop
                                                    ? paddingTop
                                                    : transformComponent.position.y;
                transformComponent.position.y = transformComponent.position.y > Game::mapHeight - paddingBottom
                                                    ? Game::mapHeight - paddingBottom
                                                    : transformComponent.position.y;
            }

            const bool isEntityOutOfBounds = (
                transformComponent.position.x < 0 ||
                transformComponent.position.x > Game::mapWidth ||
                transformComponent.position.y < 0 ||
                transformComponent.position.y > Game::mapHeight
            );

            if (isEntityOutOfBounds && !entity.HasTag("player")) {
                entity.Destroy();
            }
        }
    }


    void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
    }

private:
    void onEnemyHitsObstacle(const Entity enemy, const Entity obstacle) {
        if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
            auto& rigidBodyComponent = enemy.GetComponent<RigidBodyComponent>();
            auto& spriteComponent = enemy.GetComponent<SpriteComponent>();

            if (rigidBodyComponent.velocity.x != 0) {
                // invert the enemy's velocity
                rigidBodyComponent.velocity.x *= -1;
                // flip the enemy's sprite
                spriteComponent.flip = spriteComponent.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
            // do the same for y
            else if (rigidBodyComponent.velocity.y != 0) {
                rigidBodyComponent.velocity.y *= -1;
                spriteComponent.flip = spriteComponent.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
            }
        }
    }

    void onCollision(CollisionEvent& collisionEvent) {
        Entity a = collisionEvent.a;
        Entity b = collisionEvent.b;

        if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
            onEnemyHitsObstacle(a, b);
        } else if (b.BelongsToGroup("enemies") && a.BelongsToGroup("obstacles")) {
            onEnemyHitsObstacle(b, a);
        }
    }
};

#endif // MOVEMENT_SYSTEM_H
