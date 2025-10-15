#pragma once

#include "../HLX_pch.h"

namespace HLX {

enum class PNGFilter { AUTO = -1, NONE, SUB, UP, AVERAGE, PAETH };

class Image {
  public:
    virtual ~Image();
    virtual bool saveToFile(const int &IMAGE_WIDTH, const int &IMAGE_HEIGHT,
                            const std::vector<SDL_FColor> &PIXEL_COLORS,
                            const char *FILE_NAME) = 0;
};

class ImagePNG : public Image {
  public:
    ImagePNG(bool hasAlpha);

    virtual bool saveToFile(const int &IMAGE_WIDTH, const int &IMAGE_HEIGHT,
                            const std::vector<SDL_FColor> &PIXEL_COLORS,
                            const char *FILE_NAME) override;

    void setAlpha(bool hasAlpha);
    void setFilter(PNGFilter filter);
    void setCompression(int compression);

  private:
    int mComp;
};

class ImageJPG : public Image {
    ImageJPG();

    virtual bool saveToFile(const int &IMAGE_WIDTH, const int &IMAGE_HEIGHT,
                            const std::vector<SDL_FColor> &PIXEL_COLORS,
                            const char *FILE_NAME) override;

    void setQuality(int quality);

  private:
    int mQuality;
    int mComp;
};

}; // namespace HLX
