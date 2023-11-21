//
// Created by hector on 11/20/23.
//

#ifndef COLLISION_EVENT_H
#define COLLISION_EVENT_H
#include "../event_bus/event.h"
#include "../ecs/ecs.h"

class CollisionEvent: public Event {
public:
    Entity a;
    Entity b;

    CollisionEvent(const Entity a, const Entity b) : a(a), b(b) {
    }
};

#endif //COLLISION_EVENT_H
