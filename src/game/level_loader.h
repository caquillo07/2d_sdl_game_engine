//
// Created by Hector Mejia on 12/21/23.
//

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include "../asset_store/asset_store.h"
#include "../ecs/ecs.h"


class LevelLoader {
public:
    LevelLoader();

    ~LevelLoader();

    void LoadLevel(const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore,
                   SDL_Renderer* renderer, int i);
};


#endif //LEVEL_LOADER_H
