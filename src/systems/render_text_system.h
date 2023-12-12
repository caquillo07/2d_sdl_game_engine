#ifndef RENDER_TEXT_SYSTEM_H
#define RENDER_TEXT_SYSTEM_H

#include "../asset_store/asset_store.h"
#include "../ecs/ecs.h"
#include "../components/text_label_component.h"

#include <SDL2/SDL.h>

class RenderTextSystem: public System {
public:
    RenderTextSystem() {
        RequireComponent<TextLabelComponent>();
    }

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
        for(auto entity: GetEntities()) {
            const auto textLabel = entity.GetComponent<TextLabelComponent>();

            SDL_Surface* surface = TTF_RenderText_Blended(
                assetStore->GetFont(textLabel.assetID),
                textLabel.text.c_str(),
                textLabel.color
            );

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWidth = 0;
            int labelHeight = 0;
            SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);
            SDL_Rect destRect = {
                static_cast<int>(textLabel.position.x - (textLabel.isFixed ? 0 : camera.x)),
                static_cast<int>(textLabel.position.y - (textLabel.isFixed ? 0 : camera.y)),
                labelWidth,
                labelHeight
            };

            SDL_RenderCopy(renderer, texture, NULL, &destRect);
            SDL_DestroyTexture(texture);
        }
    }
};

#endif
