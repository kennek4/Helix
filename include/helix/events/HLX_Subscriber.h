#pragma once

#include "../HLX_pch.h"

namespace HLX {
class Subscriber {
  public:
    Subscriber() {};
    ~Subscriber() {};
    virtual bool onNotify(SDL_Event *event) = 0;
};
}; // namespace HLX
