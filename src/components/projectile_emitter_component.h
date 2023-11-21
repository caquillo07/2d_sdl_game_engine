//
// Created by hector on 11/20/23.
//

#ifndef PROJECTILE_EMITTER_COMPONENT_H
#define PROJECTILE_EMITTER_COMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {
    glm::vec2 velocity;
    int frequency;
    int duration;
    int hitPercentDamage;
    bool isFriendly;
    int lastEmissionTime;

    ProjectileEmitterComponent(
        const glm::vec2 velocity = glm::vec2(0),
        const int frequency = 0,
        const int duration = 10000,
        const int hitPercentDamage = 10,
        const bool isFriendly = false
    ) : velocity(velocity),
        frequency(frequency),
        duration(duration),
        hitPercentDamage(hitPercentDamage),
        isFriendly(isFriendly) {
        this->lastEmissionTime = static_cast<int>(SDL_GetTicks());
    }
};

#endif //PROJECTILE_EMITTER_COMPONENT_H
