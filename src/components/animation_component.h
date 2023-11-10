//
// Created by hector on 11/6/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_ANIMATION_COMPONENT_H
#define INC_2D_SDL_GAME_ENGINE_ANIMATION_COMPONENT_H

#include <SDL2/SDL.h>

struct AnimationComponent {
    int numFrames;
    int currentFrame;
    int frameRateSpeed;
    bool shouldLoop;
    int startTime;

    explicit AnimationComponent(
        const int numFrames = 1,
        const int frameRateSpeed = 1,
        const bool loop = true
    ) :
            numFrames(numFrames),
            currentFrame(1),
            frameRateSpeed(frameRateSpeed),
            shouldLoop(loop),
            startTime(0) {
        this->startTime = static_cast<int>(SDL_GetTicks());
    }
};

#endif //INC_2D_SDL_GAME_ENGINE_ANIMATION_COMPONENT_H
