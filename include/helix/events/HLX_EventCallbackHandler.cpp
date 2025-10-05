#include "HLX_EventCallbackHandler.h"

namespace HLX {
void EventCallbackHandler::invokeCallback(SDL_Event *event) {
    if (!mCallbacks.contains(event->type)) {
        return; // No callback for the given event type was registered
    };

    mCallbacks.at(event->type)(event);
};

void EventCallbackHandler::registerCallback(
    Uint32 eventType, std::function<void(SDL_Event *)> callback) {
    if (mCallbacks.contains(eventType)) {
        return; // Event already has an associated callback
    };

    mCallbacks.insert({eventType, callback});
};

}; // namespace HLX
