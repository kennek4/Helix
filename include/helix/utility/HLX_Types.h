#pragma once

namespace HLX {
typedef struct SDLProps {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Cursor *cursor;
} SDLProps;

typedef struct WindowProps {
    int width;
    int height;
} WindowProps;

typedef struct ToolProps {
    SDL_FColor color{0.0f, 0.0f, 0.0f, SDL_ALPHA_OPAQUE_FLOAT};
    int size{1};
} ToolProps;

typedef struct Grid {
    std::vector<SDL_FRect> frects;
    std::vector<SDL_FColor> colors;
    std::vector<char> states;
    int widthInPixels{32};
    int heightInPixels{32};
} Grid;

}; // namespace HLX
