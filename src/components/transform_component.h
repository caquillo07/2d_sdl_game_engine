#ifndef TRANSFORMS_COMPONENT_H
#define TRANSFORMS_COMPONENT_H

#include <glm/glm.hpp>

struct TransformComponent {
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;

    TransformComponent(
            glm::vec2 position = glm::vec2(0, 0),
            glm::vec2 scale = glm::vec2(1, 1),
            double rotation = 0.0f
    ) :
            position(position),
            scale(scale),
            rotation(rotation) {

    }
};

#endif // TRANSFORMS_COMPONENT_H
