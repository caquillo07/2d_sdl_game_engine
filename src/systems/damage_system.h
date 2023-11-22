//
// Created by hector on 11/20/23.
//

#ifndef DAMAGE_SYSTEM_H
#define DAMAGE_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/box_collider_component.h"
#include "../components/projectile_component.h"
#include "../event_bus/event_bus.h"
#include "../events/collision_event.h"

class DamageSystem : public System {
public:
    DamageSystem() {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
    }

private:
    void onCollision(CollisionEvent& event) {
        const Entity a = event.a;
        const Entity b = event.b;
        Logger::Log("Collision event emitted: " + std::to_string(a.GetID()) + " and " + std::to_string(b.GetID()));

        Logger::Log("first if");
        if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
            onProjectileHitsPlayer(a, b); // "a" is the projectile, "b" is the player
        }

        Logger::Log("second if");
        if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
            onProjectileHitsPlayer(b, a); // "b" is the projectile, "a" is the player
        }

        Logger::Log("third if");
        if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
            // TODO: OnProjectileHitsEnemy(...);
        }

        Logger::Log("fourth if");
        if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
            // TODO: OnProjectileHitsEnemy(...);
        }
    }

    void onProjectileHitsPlayer(const Entity projectile, const Entity player) {
        const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (!projectileComponent.isFriendly) {
            // Reduce the health of the player by the projectile hitPercentDamage
            auto& health = player.GetComponent<HealthComponent>();

            // Subtract the health of the player
            health.healthPercentage -= projectileComponent.hitPercentDamage;

            // Kills the player when health reaches zero
            if (health.healthPercentage <= 0) {
                player.Destroy();
            }

            // Destroy the projectile
            projectile.Destroy();
        }
    }

    void Update() {
    }
};

#endif //DAMAGE_SYSTEM_H
