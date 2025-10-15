#include "HLX_Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace HLX {

// --------------------- //
//         PNG           //
// --------------------- //

ImagePNG::ImagePNG(bool hasAlpha) { setAlpha(hasAlpha); };

bool ImagePNG::saveToFile(const int &IMAGE_WIDTH, const int &IMAGE_HEIGHT,
                          const std::vector<SDL_FColor> &PIXEL_COLORS,
                          const char *FILE_NAME) {
    bool success = false;

    const int CHANNELS_NEEDED = PIXEL_COLORS.size() * mComp;
    Uint8 *imageData = new Uint8[CHANNELS_NEEDED]();

    int index = 0;
    for (const SDL_FColor &PIXEL_COLOR : PIXEL_COLORS) {
        imageData[index++] = (int)(PIXEL_COLOR.r * 255.99f);
        imageData[index++] = (int)(PIXEL_COLOR.g * 255.99f);
        imageData[index++] = (int)(PIXEL_COLOR.b * 255.99f);

        if (mComp == STBI_rgb_alpha) {
            imageData[index++] = (int)(PIXEL_COLOR.a * 255.99f);
        };
    };

    success = stbi_write_png(FILE_NAME, IMAGE_WIDTH, IMAGE_HEIGHT, mComp,
                             imageData, IMAGE_WIDTH * mComp) != 0;

    delete[] imageData;
    return success;
};

void ImagePNG::setAlpha(bool hasAlpha) {
    if (hasAlpha) {
        mComp = STBI_rgb;
    } else {
        mComp = STBI_rgb_alpha;
    };
};

void ImagePNG::setFilter(PNGFilter filter) {
    stbi_write_force_png_filter = (int)filter;
};

void ImagePNG::setCompression(int compression) {
    stbi_write_png_compression_level = compression;
};

// --------------------- //
//         JPG           //
// --------------------- //

ImageJPG::ImageJPG() {
    mQuality = 75;
    mComp = STBI_rgb;
}

bool ImageJPG::saveToFile(const int &IMAGE_WIDTH, const int &IMAGE_HEIGHT,
                          const std::vector<SDL_FColor> &PIXEL_COLORS,
                          const char *FILE_NAME) {

    bool success = false;

    const int CHANNELS_NEEDED = PIXEL_COLORS.size() * mComp;
    Uint8 *imageData = new Uint8[CHANNELS_NEEDED]();

    int index = 0;
    for (const SDL_FColor &PIXEL_COLOR : PIXEL_COLORS) {
        imageData[index++] = (int)(PIXEL_COLOR.r * 255.99f);
        imageData[index++] = (int)(PIXEL_COLOR.g * 255.99f);
        imageData[index++] = (int)(PIXEL_COLOR.b * 255.99f);
    };

    success = stbi_write_jpg(FILE_NAME, IMAGE_WIDTH, IMAGE_HEIGHT, mComp,
                             imageData, mQuality) != 0;

    delete[] imageData;
    return success;
};

void ImageJPG::setQuality(int quality) { mQuality = quality; };

}; // namespace HLX
