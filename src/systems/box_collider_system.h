//
// Created by hector on 11/18/23.
//

#ifndef BOX_COLLIDER_SYSTEM_H
#define BOX_COLLIDER_SYSTEM_H

#include "../components/transform_component.h"
#include "../components/box_collider_component.h"
#include "../ecs/ecs.h"
#include "../event_bus/event_bus.h"
#include "../events/collision_event.h"

class BoxColliderSystem : public System {
public:
    BoxColliderSystem() {
        RequireComponent<BoxColliderComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(const std::unique_ptr<EventBus>& eventBus) const {
        auto entities = GetEntities();
        for (auto i = entities.begin(); i != entities.end(); ++i) {
            Entity entity = *i;
            const auto& aTransform = entity.GetComponent<TransformComponent>();
            const auto& aCollider = entity.GetComponent<BoxColliderComponent>();
            for (auto j = i; j != entities.end(); ++j) {
                Entity otherEntity = *j;
                if (entity == otherEntity) {
                    continue;
                }
                const auto otherTransform = otherEntity.GetComponent<TransformComponent>();
                const auto otherCollider = otherEntity.GetComponent<BoxColliderComponent>();

                if (CheckAABBCollision(
                    aTransform.position.x + aCollider.offset.x,
                    aTransform.position.y + aCollider.offset.y,
                    aCollider.width,
                    aCollider.height,
                    otherTransform.position.x + otherCollider.offset.x,
                    otherTransform.position.y + otherCollider.offset.y,
                    otherCollider.width,
                    otherCollider.height
                )) {
                    // emit event
                    eventBus->EmitEvent<CollisionEvent>(entity, otherEntity);
                }
            }
        }
    }

    static bool CheckAABBCollision(
        const double aX, const double aY, const int aWidth, const int aHeight,
        const double bX, const double bY, const int bWidth, const int bHeight
    ) {
        return aX < bX + bWidth && aX + aWidth > bX && aY < bY + bHeight && aY + aHeight > bY;
    }
};

#endif //BOX_COLLIDER_SYSTEM_H
