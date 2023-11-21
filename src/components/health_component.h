//
// Created by hector on 11/20/23.
//

#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

struct HealthComponent {
    int healthPercentage;

    HealthComponent(const int healthPercentage = 0) : healthPercentage(healthPercentage) {
    }
};

#endif //HEALTH_COMPONENT_H
