#pragma once

#include "../HLX_pch.h"

namespace HLX {

typedef struct PaletteData {
    SDL_FColor color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
    int brushSize{1};
} PaletteData;

class Palette {
  public:
    Palette() {};
    ~Palette() {};

    PaletteData *getPaletteData() { return &mData; };

    void setBrushSize(int newBrushSize) {
        if (newBrushSize != 0) {
            mData.brushSize = newBrushSize;
        };
    };

    void setColor(SDL_FColor newColor) { mData.color = newColor; };

  private:
    PaletteData mData;
};

}; // namespace HLX
