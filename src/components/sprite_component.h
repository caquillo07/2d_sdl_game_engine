//
// Created by hector on 11/6/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H
#define INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H

#include <string>
#include <utility>

struct SpriteComponent {
    std::string textureAssetID;

    int width;
    int height;
    int zIndex;
    bool isFixed;
    SDL_Rect srcRect;

    SpriteComponent(
            std::string assetID = "",
            const int width = 0,
            const int height = 0,
            const int zIndex = 0,
            const bool isFixed = false,
            const int sourceRectX = 0,
            const int sourceRectY = 0
    ) :
            textureAssetID(std::move(assetID)),
            width(width),
            height(height),
            zIndex(zIndex),
            isFixed(isFixed),
            srcRect(SDL_Rect{sourceRectX, sourceRectY, width, height}) {

    }
};

#endif //INC_2D_SDL_GAME_ENGINE_SPRITE_COMPONENT_H
