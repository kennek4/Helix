#include "HLX_Renderer.h"

namespace HLX {

Renderer::Renderer(SDL_Renderer *renderer) : mSDLRenderer(renderer) {
    std::string bgPath = SDL_GetBasePath();
    bgPath += "assets/checkerboard.bmp";

    SDL_Surface *bgSurface = SDL_LoadBMP(bgPath.c_str());

    mBackgroundTexture = SDL_CreateTextureFromSurface(mSDLRenderer, bgSurface);
    SDL_SetTextureBlendMode(mBackgroundTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaModFloat(mBackgroundTexture, 0.05f);
    SDL_DestroySurface(bgSurface);
};

Renderer::~Renderer() { SDL_DestroyTexture(mBackgroundTexture); };

void Renderer::clearScreen() {
    SDL_SetRenderDrawColorFloat(mSDLRenderer, 0.8f, 0.8f, 0.8f,
                                SDL_ALPHA_OPAQUE);
    SDL_RenderClear(mSDLRenderer);
};

void Renderer::createBackground(const SDL_FRect &backgroundFRect,
                                const float backgroundScale) {
    SDL_RenderTextureTiled(mSDLRenderer, mBackgroundTexture, NULL,
                           backgroundScale, &backgroundFRect);
};

void Renderer::createGrid(const Grid &GRID) {
    for (int i = 0; i < (GRID.widthInPixels * GRID.heightInPixels); i++) {
        if (GRID.states.at(i) != false) {
            const SDL_FColor &PIXEL_COLOR = GRID.colors.at(i);
            SDL_SetRenderDrawColorFloat(mSDLRenderer, PIXEL_COLOR.r,
                                        PIXEL_COLOR.g, PIXEL_COLOR.b,
                                        PIXEL_COLOR.a);
            SDL_RenderFillRect(mSDLRenderer, &GRID.frects.at(i));
        } else {
            SDL_SetRenderDrawColorFloat(mSDLRenderer, 0.0f, 0.0f, 0.0f, 0.25f);
            SDL_RenderRect(mSDLRenderer, &GRID.frects.at(i));
        };
    };
};

void Renderer::render() { SDL_RenderPresent(mSDLRenderer); };

}; // namespace HLX
