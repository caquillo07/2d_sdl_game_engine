//
// Created by hector on 11/6/23.
//

#ifndef INC_2D_SDL_GAME_ENGINE_ASSET_STORE_H
#define INC_2D_SDL_GAME_ENGINE_ASSET_STORE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <string>

class AssetStore {
private:
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, TTF_Font*> fonts;

public:
    AssetStore();
    ~AssetStore();

    void ClearAssets();

    void AddTexture(SDL_Renderer* renderer, const std::string& assetID, const std::string& filePath);
    SDL_Texture* GetTexture(const std::string& assetID);

    void AddFont(const std::string& assetID, const std::string& filePath, int fontSize);
    TTF_Font* GetFont(const std::string& assetID);
};

#endif //INC_2D_SDL_GAME_ENGINE_ASSET_STORE_H
