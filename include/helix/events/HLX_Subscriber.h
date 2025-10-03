#pragma once

#include <SDL3/SDL_events.h>

namespace HLX {
class Subscriber {
  public:
    virtual ~Subscriber() = default;
    virtual bool onNotify(SDL_Event *event) = 0;
};
}; // namespace HLX
