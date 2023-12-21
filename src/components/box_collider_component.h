//
// Created by hector on 11/18/23.
//

#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H
#include <glm/glm.hpp>


struct BoxColliderComponent {
    int width;
    int height;
    glm::vec2 offset;

    explicit BoxColliderComponent(
        const int width = 0, const int height = 0, const glm::vec2 offset = glm::vec2(0)
    ) : width(width), height(height), offset(offset) {
    }
};


#endif //BOX_COLLIDER_COMPONENT_H
