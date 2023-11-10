#include "ecs.h"
#include "../logger/logger.h"
#include <algorithm>

// Components
int BaseComponent::nextID = 0;

// Entity
Entity::Entity(const int id) : id(id), registry(nullptr) {
}

int Entity::GetID() const { return this->id; }

// Systems
void System::AddEntity(const Entity entity) { this->entities.push_back(entity); }

void System::RemoveEntity(Entity entity) {
    // I hate this, next time use a boring for loop
    this->entities.erase(
        std::remove_if(
            entities.begin(), entities.end(),
            [&entity](Entity e) {
                return e == entity;
            }
        ),
        entities.end()
    );
}

std::vector<Entity> System::GetEntities() const { return this->entities; }

const Signature& System::GetComponentSignature() const {
    return this->componentSignature;
}

Entity Registry::CreateEntity() {
    const int entityID = numEntities++;
    if (entityID >= static_cast<int>(entityComponentSignatures.size())) {
        entityComponentSignatures.resize(entityID + 1);
    }
    Entity entity(entityID);
    entity.registry = this;
    entitiesToCreate.insert(entity);
    Logger::Log("Entity created with ID = " + std::to_string(entityID));
    return entity;
}

void Registry::Update() {
    for (auto& entity: entitiesToCreate) {
        AddEntityToSystems(entity);
    }
    entitiesToCreate.clear();

    for (auto& entity: entitiesToDestroy) {
        DestroyEntity(entity);
    }
    entitiesToDestroy.clear();
}

void Registry::AddEntityToSystems(const Entity entity) const {
    const auto entityID = entity.GetID();
    const auto& entityComponentSignature = entityComponentSignatures[entityID];

    for (auto& system: systems) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();
        const bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if (isInterested) {
            system.second->AddEntity(entity);
        }
    }
}

void Registry::DestroyEntity(Entity entity) {
}
void Registry::AddEntityToDestroy(Entity entity) {
}
