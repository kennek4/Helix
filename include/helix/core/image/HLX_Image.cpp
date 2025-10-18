#include "HLX_Image.h"
#include "stb/stb_image.h"
#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace HLX {
namespace Image {

bool validFileName(std::string &fileName) {
    if (fileName.empty() || fileName.length() > 255)
        return false;

    // HACK: Hardcoding to validate only .png files
    const std::regex validChars{"^[\\w,\\s-]+\\.png"};

    return std::regex_match(fileName, validChars);
};

bool savePNG(const int &imageWidth, const int &imageHeight,
             const std::vector<SDL_FColor> &pixelColors, const char *fileName) {

    int comp = STBI_rgb_alpha;
    bool success = false;

    const int CHANNELS_NEEDED = pixelColors.size() * comp;
    Uint8 *imageData = new Uint8[CHANNELS_NEEDED]();

    int index = 0;
    for (const SDL_FColor &pixelColor : pixelColors) {
        imageData[index++] = (int)(pixelColor.r * 255.99f);
        imageData[index++] = (int)(pixelColor.g * 255.99f);
        imageData[index++] = (int)(pixelColor.b * 255.99f);

        if (comp == STBI_rgb_alpha) {
            imageData[index++] = (int)(pixelColor.a * 255.99f);
        };
    };

    success = stbi_write_png(fileName, imageWidth, imageHeight, comp, imageData,
                             imageWidth * comp) != 0;

    delete[] imageData;
    return success;
};

bool saveJPG(const int &imageWidth, const int &imageHeight,
             const std::vector<SDL_FColor> &pixelColors, const char *fileName) {

    int comp = STBI_rgb_alpha;
    int quality = 75;
    bool success = false;

    const int CHANNELS_NEEDED = pixelColors.size() * comp;
    Uint8 *imageData = new Uint8[CHANNELS_NEEDED]();

    int index = 0;
    for (const SDL_FColor &PIXEL_COLOR : pixelColors) {
        imageData[index++] = (int)(PIXEL_COLOR.r * 255.99f);
        imageData[index++] = (int)(PIXEL_COLOR.g * 255.99f);
        imageData[index++] = (int)(PIXEL_COLOR.b * 255.99f);
    };

    success = stbi_write_jpg(fileName, imageWidth, imageHeight, comp, imageData,
                             quality) != 0;

    delete[] imageData;
    return success;
};

} // namespace Image
}; // namespace HLX
