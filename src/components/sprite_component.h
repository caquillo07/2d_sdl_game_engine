//
// Created by hector on 11/6/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H
#define INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H

#include <SDL2/SDL_render.h>
#include <string>

struct SpriteComponent {
//    SDL_Texture* texture;
//    SDL_Rect sourceRectangle;
//    SDL_Rect destinationRectangle;

    int width;
    int height;

    SpriteComponent() : width(50), height(50) {
//        sourceRectangle = {0, 0, width, height};
//        destinationRectangle = {0, 0, width, height};
    }

//    SpriteComponent(
//            SDL_Texture* texture,
//            SDL_Rect sourceRectangle = {0, 0, 32, 32},
//            SDL_Rect destinationRectangle = {0, 0, 32, 32}
//    ) :
//            texture(texture),
//            sourceRectangle(sourceRectangle),
//            destinationRectangle(destinationRectangle) {
//
//    }
};

#endif //INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H
