//
// Created by hector on 11/20/23.
//

#ifndef DAMAGE_SYSTEM_H
#define DAMAGE_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/box_collider_component.h"
#include "../event_bus/event_bus.h"
#include "../events/collision_event.h"

class DamageSystem : public System {
public:
    DamageSystem() {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent& e) {
        Logger::Log(
            "Collision detected between entities " + std::to_string(e.a.GetID()) + " and " + std::to_string(e.b.GetID())
        );
        // e.a.Destroy();
        // e.b.Destroy();
    }

    void Update() {
    }
};

#endif //DAMAGE_SYSTEM_H
