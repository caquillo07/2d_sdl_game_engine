//
// Created by hector on 11/6/23.
//
#include "asset_store.h"
#include "../logger/logger.h"
#include "SDL2/SDL_image.h"

AssetStore::AssetStore() {
    Logger::Log("Asset store constructor");
}

AssetStore::~AssetStore() {
    ClearAssets();
    Logger::Log("Asset store destructor");
}

void AssetStore::ClearAssets() {
    for (const auto& texture: textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();

    for (auto font: fonts) {
        TTF_CloseFont(font.second);
    }
    fonts.clear();
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

void AssetStore::AddFont(const std::string& assetID, const std::string& filePath, int fontSize) {
    fonts.emplace(assetID, TTF_OpenFont(filePath.c_str(), fontSize));
}

TTF_Font* AssetStore::GetFont(const std::string& assetID) {
    return fonts[assetID];
}
