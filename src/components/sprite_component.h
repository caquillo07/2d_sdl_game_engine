//
// Created by hector on 11/6/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H
#define INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H

#include <SDL2/SDL_render.h>
#include <string>

struct SpriteComponent {
    std::string textureAssetID;

    int width;
    int height;
    int zIndex;
    SDL_Rect srcRect;

    SpriteComponent(
            std::string assetID = "", 
            int width = 0,
            int height = 0,
            int zIndex = 0,
            int sourceRectX = 0,
            int sourceRectY = 0
    ) :
            textureAssetID(assetID),
            width(width),
            height(height),
            zIndex(zIndex),
            srcRect(SDL_Rect{sourceRectX, sourceRectY, width, height}) {

    }
};

#endif //INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H
