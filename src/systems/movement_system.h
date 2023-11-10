#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../logger/logger.h"

class MovementSystem: public System { //: public System {
  public:
    MovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void Update(const float deltaTime) const {
        for (auto entity : GetEntities()) {
            auto& transformComponent = entity.GetComponent<TransformComponent>();
            const auto& rigidBodyComponent = entity.GetComponent<RigidBodyComponent>();

            transformComponent.position += rigidBodyComponent.velocity * deltaTime;
        }
    }
};

#endif // MOVEMENT_SYSTEM_H
