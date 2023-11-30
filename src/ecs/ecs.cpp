#include "ecs.h"
#include "../logger/logger.h"
#include <algorithm>

// Components
int BaseComponent::nextID = 0;

// Entity
Entity::Entity(const int id) : id(id), registry(nullptr) {
}

void Entity::Destroy() const {
    registry->DestroyEntity(*this);
}

int Entity::GetID() const { return this->id; }

void Entity::Tag(const std::string& tag) const {
    registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const {
    Logger::Log("HasTag: " + tag);
    return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group) const {
    registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const {
    Logger::Log("BelongsToGroup: " + group);
    return registry->EntityBelongsToGroup(*this, group);
}

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
    int entityID;
    if (this->freeIDs.empty()) {
        entityID = numEntities++;
        if (entityID >= static_cast<int>(entityComponentSignatures.size())) {
            entityComponentSignatures.resize(entityID + 1);
        }
    } else {
        // reuse an id from the list of recently destroyed entities
        entityID = freeIDs.front();
        freeIDs.pop_front();
    }

    Entity entity(entityID);
    entity.registry = this;
    entitiesToCreate.insert(entity);

    // Logger::Log("Entity created with ID = " + std::to_string(entityID));
    return entity;
}

void Registry::Update() {
    for (auto& entity: entitiesToCreate) {
        AddEntityToSystems(entity);
    }
    entitiesToCreate.clear();

    for (auto& entity: entitiesToDestroy) {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetID()].reset();

        // remove the entity from the component pools
        for (const auto& pool: componentPools) {
            if (!pool) {
                continue;
            }
            pool->RemoveEntityFromPool(entity.GetID());
        }

        this->freeIDs.push_back(entity.GetID());

        RemoveEntityTag(entity);
        RemoveEntityGroup(entity);
    }
    entitiesToDestroy.clear();
}

void Registry::DestroyEntity(const Entity entity) {
    this->entitiesToDestroy.insert(entity);
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

void Registry::TagEntity(Entity entity, const std::string& tag) {
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetID(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string& tag) const {
    if (tagPerEntity.find(entity.GetID()) == tagPerEntity.end()) {
        return false;
    }
    return entityPerTag.find(tag)->second == entity;
}

Entity Registry::GetEntityByTag(const std::string& tag) const {
    return entityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity) {
    const auto taggedEntity = tagPerEntity.find(entity.GetID());
    if (taggedEntity != tagPerEntity.end()) {
        const auto tag = taggedEntity->second;
        entityPerTag.erase(tag);
        tagPerEntity.erase(taggedEntity);
    }
}

void Registry::GroupEntity(Entity entity, const std::string& group) {
    entitiesPerGroup.emplace(group, std::set<Entity>());
    entitiesPerGroup[group].emplace(entity);
    groupPerEntity.emplace(entity.GetID(), group);
}

bool Registry::EntityBelongsToGroup(const Entity entity, const std::string& group) const {
    if (entitiesPerGroup.find(group) == entitiesPerGroup.end()) {
        return false;
    }
    auto groupEntities = entitiesPerGroup.at(group);
    return groupEntities.find(entity.GetID()) != groupEntities.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const {
    if (entitiesPerGroup.find(group) == entitiesPerGroup.end()) {
        return {};
    }
    auto& setOfEntities = entitiesPerGroup.at(group);
    return {setOfEntities.begin(), setOfEntities.end()};
}

void Registry::RemoveEntityGroup(const Entity entity) {
    // if in group, remove entity from group management
    const auto groupedEntity = groupPerEntity.find(entity.GetID());
    if (groupedEntity != groupPerEntity.end()) {
        const auto group = entitiesPerGroup.find(groupedEntity->second);
        if (group != entitiesPerGroup.end()) {
            const auto entityInGroup = group->second.find(entity);
            if (entityInGroup != group->second.end()) {
                group->second.erase(entityInGroup);
            }
        }
        groupPerEntity.erase(groupedEntity);
    }
}

void Registry::RemoveEntityFromSystems(const Entity entity) const {
    for (const auto& system: systems) {
        system.second->RemoveEntity(entity);
    }
}
