#pragma once

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <array>
#include <stdexcept>
#include <utility>
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
    SDL_FColor primary = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
    SDL_FColor secondary = SDL_FColor{1.0f, 1.0f, 1.0f, 1.0f};
    SDL_FColor *currentColor{&primary};
    int size{1};
} ToolProps;

typedef struct GuiProps {
    bool isKeybindMenuActive{false};
    bool isCreditsScreenActive{false};
    bool isSaveScreenActive{false};
} GuiProps;

typedef struct Grid {
    std::vector<SDL_FRect> frects;
    std::vector<SDL_FColor> colors;
    std::vector<char> states;
    int widthInPixels{32};
    int heightInPixels{32};
} Grid;

template <typename Key, typename Value, size_t Size> struct ConstMap {
    std::array<std::pair<Key, Value>, Size> data;

    [[nodiscard]] constexpr Value operator[](const Key &key) const {
        for (const std::pair<Key, Value> &pair : data) {
            if (pair.first == key) {
                return pair.second;
            };
        };

        throw std::out_of_range("The given key was not found!");
    };
};

}; // namespace HLX
