#ifndef ECS_H
#define ECS_H

class Component {
  public:
    ~Component();
};

class Entity {
  private:
    int id;

  public:
    Entity(int id);
    int GetID() const;
};

// the system processes entities that contain a specific signature
class System {};

class Registry {};

#endif // ECS_H
