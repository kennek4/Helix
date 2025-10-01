#include "HLX_Window.h"

static bool isMainWindowInitialized = false;

namespace HLX {

Window::Window() {};

Window::~Window() {

};

SDLData &Window::getSDLData() { return mSDLData; };

}; // namespace HLX
