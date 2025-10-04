#pragma once

#include "../HLX_pch.h"
#include "HLX_Subscriber.h"

namespace HLX {

class EventCallbackHandler {
  public:
    EventCallbackHandler(Subscriber *subscriber) : mSubscriber(subscriber) {};
    ~EventCallbackHandler() {};

    void invokeCallback(SDL_Event *event);
    void registerCallback(Uint32 eventType,
                          std::function<void(SDL_Event *)> callback);

  private:
    Subscriber *mSubscriber;
    std::map<Uint32, std::function<void(SDL_Event *)>> mCallbacks{};
};
}; // namespace HLX
