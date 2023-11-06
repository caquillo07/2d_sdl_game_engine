#ifndef TRANSFORMS_COMPONENT_H
#define TRANSFORMS_COMPONENT_H

#include <glm/glm.hpp>

struct TransformsComponent {
    glm::vec2 position;
    glm::vec2 scale;
    float rotation;
};

#endif // TRANSFORMS_COMPONENT_H
