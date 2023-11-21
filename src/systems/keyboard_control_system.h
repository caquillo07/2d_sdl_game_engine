//
// Created by hector on 11/20/23.
//

#ifndef KEYBOARD_MOVEMENT_SYSTEM_H
#define KEYBOARD_MOVEMENT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../event_bus/event_bus.h"
#include "../events/key_pressed_event.h"
#include "../components/sprite_component.h"
#include "../components/rigid_body_component.h"
#include "../components/keyword_controlled_component.h"

class KeyboardControlSystem : public System {
public:
    KeyboardControlSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidBodyComponent>();
        RequireComponent<KeywordControlledComponent>();
    }

    void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

private:
    void OnKeyPressed(KeyPressedEvent& e) {
        for (auto entity: GetEntities()) {
            const auto keyboardControl = entity.GetComponent<KeywordControlledComponent>();
            auto& sprite = entity.GetComponent<SpriteComponent>();
            auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

            switch (e.key) {
                case SDLK_UP:
                    rigidBody.velocity = keyboardControl.upVelocity;
                    sprite.srcRect.y = sprite.height * 0;
                    break;
                case SDLK_RIGHT:
                    rigidBody.velocity = keyboardControl.rightVelocity;
                    sprite.srcRect.y = sprite.height * 1;
                    break;
                case SDLK_DOWN:
                    rigidBody.velocity = keyboardControl.downVelocity;
                    sprite.srcRect.y = sprite.height * 2;
                    break;
                case SDLK_LEFT:
                    rigidBody.velocity = keyboardControl.leftVelocity;
                    sprite.srcRect.y = sprite.height * 3;
                    break;
                default: ;
            }
        }
    }

    void Update() {
    }
};

#endif //KEYBOARD_MOVEMENT_SYSTEM_H
