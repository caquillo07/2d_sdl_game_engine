#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <deque>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <SDL2/SDL_hints.h>
#include <cstdio>
#include <iostream>

#include "../logger/logger.h"

constexpr unsigned int MAX_COMPONENTS = 32;
// we use a bitset (1s and 0s) to keep track of which components an entity has,
// and also helps keep track of which entities a system should process
typedef std::bitset<MAX_COMPONENTS> Signature;

struct BaseComponent {
protected:
    static int nextID;
};

// used to assign unique id to a component type
template<typename T>
class Component : public BaseComponent {
    // returns the unique ID of Component<T>
public:
    static int GetID() {
        static int id = nextID++;
        return id;
    }
};

class Entity {
private:
    int id;

public:
    Entity(int id);
    Entity(const Entity& other) = default; // this actually calls the = operation under the hood
    void Destroy() const;
    int GetID() const;

    // Manage entity tags and groups
    void Tag(const std::string& tag) const;
    bool HasTag(const std::string& tag) const;
    void Group(const std::string& group) const;
    bool BelongsToGroup(const std::string& group) const;

    Entity& operator =(const Entity& other) = default;
    bool operator ==(const Entity& other) const { return this->id == other.id; }
    bool operator !=(const Entity& other) const { return this->id != other.id; }
    bool operator <(const Entity& other) const { return this->id < other.id; }
    bool operator >(const Entity& other) const { return this->id > other.id; }

    template<typename TComponent, typename... TComponentArgs>
    void AddComponent(TComponentArgs&&... args);

    template<typename TComponent>
    void RemoveComponent() const;

    template<typename TComponent>
    bool HasComponent() const;

    template<typename TComponent>
    TComponent& GetComponent() const;

    // hold a pointer to the entity's owner registry
    // this is a cyclic dependency, but it's ok because this is a demo project.
    // It is important to understand the risks here though
    class Registry* registry;
};

// the system processes entities that contain a specific signature
class System {
private:
    Signature componentSignature;
    std::vector<Entity> entities;

public:
    System() = default;
    ~System() = default;

    void AddEntity(Entity entity);
    void RemoveEntity(Entity entity);

    std::vector<Entity> GetEntities() const;
    const Signature& GetComponentSignature() const;

    template<typename TComponent>
    void RequireComponent();
};

class BasePool {
public:
    virtual ~BasePool() = default;
    virtual void RemoveEntityFromPool(int entityID) = 0;
};

template<typename T>
class Pool : public BasePool {
private:
    std::vector<T> data;
    int size;
    std::unordered_map<int, int> entityIDToIndex;
    std::unordered_map<int, int> indexToEntityID;

public:
    Pool(int capacity = 100) {
        this->size = 0;
        data.resize(capacity);
    }

    ~Pool() override = default;

    bool IsEmpty() const {
        return size == 0;
    }

    int GetSize() const {
        return size;
    }

    void Clear() {
        data.clear();
        entityIDToIndex.clear();
        indexToEntityID.clear();
        size = 0;
    }

    // void Add(T component) {
    //     data.push_back(component);
    // }

    void Set(int entityID, T object) {
        if (entityIDToIndex.find(entityID) != entityIDToIndex.end()) {
            int index = entityIDToIndex[entityID];
            data[index] = object;
            return;
        }

        int index = size;
        entityIDToIndex.emplace(entityID, index);
        indexToEntityID.emplace(index, entityID);
        if (index >= static_cast<int>(data.capacity())) {
            data.resize(size * 2);
        }

        data[index] = object;
        size++;
    }

    void Remove(const int entityID) {
        const int indexOfRemoved = entityIDToIndex[entityID];
        const int indexOfLast = size - 1;
        data[indexOfRemoved] = data[indexOfLast];

        const int entityIDOfLastEmenent = indexToEntityID[indexOfLast];
        entityIDToIndex[entityIDOfLastEmenent] = indexOfRemoved;
        indexToEntityID[indexOfRemoved] = entityIDOfLastEmenent;

        entityIDToIndex.erase(entityID);
        indexToEntityID.erase(indexOfLast);

        size--;
    }

    void RemoveEntityFromPool(const int entityID) override {
        if (entityIDToIndex.find(entityID) != entityIDToIndex.end()) {
            Remove(entityID);
        }
    }

    T& Get(const int entityID) {
        int index = entityIDToIndex[entityID];
        return static_cast<T&>(data[index]);
    }

    T& operator [](unsigned int index) {
        return data[index];
    }
};

class Registry {
private:
    int numEntities = 0;

    // vector of component pools, each pool contains a all the data for a certain
    // component type.
    // Index = component type ID
    // Pool Index = entity ID
    std::vector<std::shared_ptr<BasePool>> componentPools;

    // Vector of component signatures per entity, saying which component is "on"
    // for a given entity
    // index = entityID
    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    std::set<Entity> entitiesToCreate;
    std::set<Entity> entitiesToDestroy;
    std::deque<int> freeIDs;

    // Entity tags (one tag name per entity)
    std::unordered_map<std::string, Entity> entityPerTag;
    std::unordered_map<int, std::string> tagPerEntity;

    // Entity groups (a set of entities per group name)
    std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
    std::unordered_map<int, std::string> groupPerEntity;

public:
    Registry() = default;
    ~Registry() = default;

    void Update();

    // Entities
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    void RemoveEntityFromSystems(Entity entity) const;
    void AddEntityToSystems(Entity entity) const;

    // Tag management
    void TagEntity(Entity entity, const std::string& tag);
    bool EntityHasTag(Entity entity, const std::string& tag) const;
    Entity GetEntityByTag(const std::string& tag) const;
    void RemoveEntityTag(Entity entity);

    // Group management
    void GroupEntity(Entity entity, const std::string& group);
    bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
    std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
    void RemoveEntityGroup(Entity entity);

    // Components
    template<typename TComponent, typename... TComponentArgs>
    void AddComponent(Entity entity, TComponentArgs&&... args);

    template<typename TComponent>
    void RemoveComponent(Entity entity);

    template<typename TComponent>
    bool HasComponent(Entity entity) const;

    template<typename TComponent>
    TComponent& GetComponent(Entity entity) const;

    // Systems
    template<typename TSystem, typename... TSystemArgs>
    void AddSystem(TSystemArgs&&... args);

    template<typename TSystem>
    void RemoveSystem();

    template<typename TSystem>
    TSystem& GetSystem() const;

    template<typename TSystem>
    bool HasSystem() const;
};

template<typename TComponent, typename... TComponentArgs>
void Entity::AddComponent(TComponentArgs&&... args) {
    this->registry->AddComponent<TComponent>(*this, std::forward<TComponentArgs>(args)...);
}

template<typename TComponent>
void Entity::RemoveComponent() const {
    this->registry->RemoveComponent<TComponent>(*this);
}

template<typename TComponent>
bool Entity::HasComponent() const {
    return this->registry->HasComponent<TComponent>(*this);
}

template<typename TComponent>
TComponent& Entity::GetComponent() const {
    return this->registry->GetComponent<TComponent>(*this);
}

template<typename TComponent>
void System::RequireComponent() {
    const auto componentID = Component<TComponent>::GetID();
    this->componentSignature.set(componentID);
}

template<typename TComponent, typename... TComponentArgs>
void Registry::AddComponent(const Entity entity, TComponentArgs&&... args) {
    const auto componentID = Component<TComponent>::GetID();
    const auto entityID = entity.GetID();

    if (componentID >= static_cast<int>(componentPools.size())) {
        componentPools.resize(componentID + 1, nullptr);
    }

    if (!componentPools[componentID]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool(new Pool<TComponent>);
        componentPools[componentID] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(
        componentPools[componentID]
    );

    TComponent newComponent(std::forward<TComponentArgs>(args)...);
    componentPool->Set(entityID, newComponent);

    entityComponentSignatures[entityID].set(componentID);

    // Logger::Log(
    //     "Component ID = " + std::to_string(componentID) +
    //     " added to entity ID = " + std::to_string(entityID)
    // );
    printf("COMPONENT ID = %d ----> POOL SIZE = %d\n", componentID, componentPool->GetSize());
}

template<typename TComponent>
void Registry::RemoveComponent(const Entity entity) {
    const auto componentID = Component<TComponent>::GetID();
    const auto entityID = entity.GetID();

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(
        componentPools[componentID]
    );
    componentPool->Remove(entityID);

    entityComponentSignatures[entityID].set(componentID, false);
    Logger::Log(
        "Component ID = " + std::to_string(componentID) +
        " removed from entity ID = " + std::to_string(entityID)
    );
}

template<typename TComponent>
bool Registry::HasComponent(const Entity entity) const {
    const auto componentID = Component<TComponent>::GetID();
    const auto entityID = entity.GetID();

    return entityComponentSignatures[entityID].test(componentID);
}

template<typename TComponent>
TComponent& Registry::GetComponent(const Entity entity) const {
    const auto componentID = Component<TComponent>::GetID();
    const auto entityID = entity.GetID();

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(
        componentPools[componentID]
    );

    return componentPool->Get(entityID);
}

template<typename TSystem, typename... TSystemArgs>
void Registry::AddSystem(TSystemArgs&&... args) {
    const auto systemID = std::type_index(typeid(TSystem));
    systems.insert(std::make_pair(systemID, std::make_shared<TSystem>(std::forward<TSystemArgs>(args)...)));
}

template<typename TSystem>
void Registry::RemoveSystem() {
    const auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template<typename TSystem>
bool Registry::HasSystem() const {
    const auto system = systems.find(std::type_index(typeid(TSystem)));
    return system != systems.end();
}

template<typename TSystem>
TSystem& Registry::GetSystem() const {
    const auto system = systems.find(std::type_index(typeid(TSystem)));
    return *std::static_pointer_cast<TSystem>(system->second);
}

#endif // ECS_H
