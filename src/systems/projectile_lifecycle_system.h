//
// Created by hector on 11/20/23.
//

#ifndef PROJECTILE_LIFECYCLE_SYSTEM_H
#define PROJECTILE_LIFECYCLE_SYSTEM_H
#include "../components/projectile_component.h"
#include "../ecs/ecs.h"

class ProjectileLifecycleSystem : public System {
public:
    ProjectileLifecycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update() {
        for (auto entity: GetEntities()) {
            const auto projectileComponent = entity.GetComponent<ProjectileComponent>();
            if (static_cast<int>(SDL_GetTicks()) - projectileComponent.startTime > projectileComponent.duration) {
                entity.Destroy();
            }
        }
    }
};

#endif //PROJECTILE_LIFECYCLE_SYSTEM_H
