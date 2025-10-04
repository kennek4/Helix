#include "HLX_EventCallbackHandler.h"
#include <SDL3/SDL_log.h>
#include <functional>

namespace HLX {
void EventCallbackHandler::invokeCallback(SDL_Event *event) {
    if (!mCallbacks.contains(event->type)) {
        SDL_Log("Event of ID, %d, has no callback", event->type);
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
