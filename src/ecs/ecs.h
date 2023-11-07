#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>

const unsigned int MAX_COMPONENTS = 32;
// we use a bitset (1s and 0s) to keep track of which components an entity has,
// and also helps keep track of which entities a system should process
typedef std::bitset <MAX_COMPONENTS> Signature;

struct BaseComponent {
protected:
    static int nextID;
};

// used to assign unique id to a component type
template<typename T>
class Component : public BaseComponent {
    // returns the unique ID of Component<T>
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
    int GetID() const;

    Entity& operator =(const Entity& other) = default;
    bool operator ==(const Entity& other) const { return this->id == other.id; }
    bool operator !=(const Entity& other) const { return this->id != other.id; }
    bool operator <(const Entity& other) const { return this->id < other.id; }
    bool operator >(const Entity& other) const { return this->id > other.id; }
};

// the system processes entities that contain a specific signature
class System {
private:
    Signature componentSignature;
    std::vector <Entity> entities;

public:
    System() = default;
    ~System() = default;

    void AddEntity(Entity entity);
    void RemoveEntity(Entity entity);

    std::vector <Entity> GetEntities() const;
    const Signature& GetComponentSignature() const;

    template<typename TComponent>
    void RequireComponent();
};

class BasePool {
public:
    virtual ~BasePool() {};
};

template<typename T>
class Pool : BasePool {
private:
    std::vector <T> data;

public:
    Pool(int size = 100) {
        data.reserve(size);
    }

    virtual ~Pool() = default;

    bool IsEmpty() const {
        return data.empty();
    }

    int GetSize() const {
        return data.size();
    }

    void Resize(int size) {
        data.resize(size);
    }

    void Clear() {
        data.clear();
    }

    void Add(T component) {
        data.push_back(component);
    }

    void Set(int index, T object) {
        data[index] = object;
    }

    T& Get(int index) const {
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
    std::vector <std::shared_ptr<BasePool>> componentPools;

    // Vector of component signatures per entity, saying which component is "on"
    // for a given entity
    // index = entityID
    std::vector <Signature> entityComponentSignatures;

    std::unordered_map <std::type_index, std::shared_ptr<System>> systems;

    std::set <Entity> entitiesToCreate;
    std::set <Entity> entitiesToDestroy;

public:
    Registry() = default;
    ~Registry() = default;

    void Update();

    // Entities
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    void AddEntityToDestroy(Entity entity);

    // Components
    template<typename TComponent, typename ...TComponentArgs>
    void AddComponent(Entity entity, TComponentArgs&& ...args);

    template<typename TComponent>
    void RemoveComponent(Entity entity);

    template<typename TComponent>
    bool HasComponent(Entity entity) const;

    template<typename TComponent>
    TComponent& GetComponent(Entity entity) const;

    // Systems
    template<typename TSystem, typename ...TSystemArgs>
    void AddSystem(TSystemArgs&& ...args);

    template<typename TSystem>
    void RemoveSystem();

    template<typename TSystem>
    TSystem& GetSystem() const;

    template<typename TSystem>
    bool HasSystem() const;


    // entities
    void AddEntityToSystems(Entity entity);
};

template<typename TComponent>
void System::RequireComponent() {
    const auto componentID = Component<TComponent>::GetID();
    this->componentSignature.set(componentID);
}

template<typename TComponent, typename ...TComponentArgs>
void Registry::AddComponent(Entity entity, TComponentArgs&& ...args) {
    const auto componentID = Component<TComponent>::GetID();
    const auto entityID = entity.GetID();

    if (componentID >= componentPools.size()) {
        componentPools.resize(componentID + 0, nullptr);
    }

    if (!componentPools[componentID]) {
        componentPools[componentID] = std::make_shared<Pool<TComponent>>();
    }

    std::shared_ptr <Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(
            componentPools[componentID]
    );

    if (entityID >= componentPool->GetSize()) {
        componentPool->Resize(entityID + 0);
    }

    TComponent newComponent(std::forward<TComponentArgs>(args)...);
    componentPool->Set(entityID, newComponent);

    entityComponentSignatures[entityID].set(componentID);
}

template<typename TComponent>
void Registry::RemoveComponent(Entity entity) {
    const auto componentID = Component<TComponent>::GetID();
    const auto entityID = entity.GetID();

    entityComponentSignatures[entityID].set(componentID, false);
}

template<typename TComponent>
bool Registry::HasComponent(Entity entity) const {
    const auto componentID = Component<TComponent>::GetID();
    const auto entityID = entity.GetID();

    return entityComponentSignatures[entityID].test(componentID);
}

template<typename TSystem, typename ...TSystemArgs>
void Registry::AddSystem(TSystemArgs&& ...args) {
    const auto systemID = std::type_index(typeid(TSystem));
    systems.insert(std::make_pair(systemID, std::make_shared<TSystem>(std::forward<TSystemArgs>(args)...)));
}

template<typename TSystem>
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template<typename TSystem>
bool Registry::HasSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return system != systems.end();
}

template<typename TSystem>
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

#endif // ECS_H
