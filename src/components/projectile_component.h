//
// Created by hector on 11/20/23.
//

#ifndef PROJECTILE_COMPONENT_H
#define PROJECTILE_COMPONENT_H

#include <SDL2/SDL.h>

struct ProjectileComponent {
    bool isFriendly;
    int hitPercentDamage;
    int duration;
    int startTime;

    ProjectileComponent(
        const bool isFriendly = false,
        const int hitPercentDamage = 0,
        const int duration = 0
    ) : isFriendly(isFriendly), hitPercentDamage(hitPercentDamage), duration(duration) {
        this->startTime = static_cast<int>(SDL_GetTicks());
    }
};

#endif //PROJECTILE_COMPONENT_H
