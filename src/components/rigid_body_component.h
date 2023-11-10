//
// Created by hector on 11/6/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_RIGID_BODY_COMPONENT_H
#define INC_2D_SDL_GAME_ENGINE_RIGID_BODY_COMPONENT_H

#include <glm/glm.hpp>

struct RigidBodyComponent {
    glm::vec2 velocity;

    RigidBodyComponent(const glm::vec2 velocity = glm::vec2(0, 0)) : velocity(velocity) {

    }
};

#endif //INC_2D_SDL_GAME_ENGINE_RIGID_BODY_COMPONENT_H
