//
// Created by Hector Mejia on 12/21/23.
//

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <sol/sol.hpp>
#include "../asset_store/asset_store.h"
#include "../ecs/ecs.h"


class LevelLoader {
    public:
        LevelLoader();

        ~LevelLoader();

        static void LoadLevel(
            sol::state& lua,
            const std::unique_ptr<Registry>& registry,
            const std::unique_ptr<AssetStore>& assetStore,
            SDL_Renderer* renderer,
            int levelNumber
        );
};


#endif //LEVEL_LOADER_H
