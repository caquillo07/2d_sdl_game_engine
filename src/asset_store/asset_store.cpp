//
// Created by hector on 11/6/23.
//

#include "SDL2/SDL_image.h"

#include "asset_store.h"
#include "../logger/logger.h"

AssetStore::AssetStore() {
    Logger::Log("Asset store constructor");
}

AssetStore::~AssetStore() {
    ClearAssets();
    Logger::Log("Asset store destructor");
}

void AssetStore::ClearAssets() {
    for (auto& texture: textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetID, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
        Logger::Err("Failed to load texture: " + filePath + " - " + std::string(IMG_GetError()));
        return;
    }
    // todo(hector) -  do error checking in case loading fails
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    textures.emplace(assetID, texture);
    Logger::Log("Added texture: " + assetID);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetID) {
    return textures[assetID];
}
