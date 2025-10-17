#pragma once

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
