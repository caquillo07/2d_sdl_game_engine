#include "ecs.h"

Component::~Component() {}

Entity::Entity(int id) : id(id) {}

int Entity::GetID() const { return this->id; }
