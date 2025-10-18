#pragma once

#include "../HLX_pch.h"
#include <HLX_Constants.h>

namespace HLX {
namespace Image {

bool validFileName(std::string &fileName);

bool savePNG(const int &imageWidth, const int &imageHeight,
             const std::vector<SDL_FColor> &pixelColors, const char *fileName);

bool saveJPG(const int &imageWidth, const int &imageHeight,
             const std::vector<SDL_FColor> &pixelColors, const char *fileName);

}; // namespace Image
}; // namespace HLX
