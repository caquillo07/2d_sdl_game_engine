//
// Created by hector on 11/20/23.
//

#ifndef KEYWORDCONTROLLERCOMPONENT_H
#define KEYWORDCONTROLLERCOMPONENT_H

#include <glm/glm.hpp>

struct KeywordControlledComponent {
    glm::vec2 upVelocity;
    glm::vec2 rightVelocity;
    glm::vec2 downVelocity;
    glm::vec2 leftVelocity;

    KeywordControlledComponent(
        const glm::vec2 upVelocity = glm::vec2(0),
        const glm::vec2 rightVelocity = glm::vec2(0),
        const glm::vec2 downVelocity = glm::vec2(0),
        const glm::vec2 leftVelocity = glm::vec2(0)
    ) : upVelocity(upVelocity), rightVelocity(rightVelocity), downVelocity(downVelocity), leftVelocity(leftVelocity) {
    }
    
};

#endif //KEYWORDCONTROLLERCOMPONENT_H
