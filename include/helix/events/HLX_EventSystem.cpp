#include "HLX_EventSystem.h"
#include "HLX_Subscriber.h"
#include <SDL3/SDL_log.h>

inline bool isSubscriberInTopic(Uint32 *type, HLX::Subscriber *subscriber,
                                std::map<Uint32, HLX::Topic *> *map) {
    if (!map->contains(*type)) {
        return false; // No topic with given event
    };
    return map->at(*type)->subscribers.contains(subscriber);
};

namespace HLX {

EventSystem::~EventSystem() {
    for (auto pair : mEventToTopic) {
        pair.second->subscribers.clear();
        delete pair.second;
    };

    mEventToTopic.clear();
};

void EventSystem::subscribe(Uint32 type, Subscriber *subscriber) {
    if (!mEventToTopic.contains(type)) {
        // Create the topic
        SDL_Log("EVENT TYPE ID: %d REGISTERED", type);
        mEventToTopic.insert({type, new Topic(type)});
    } else if (isSubscriberInTopic(&type, subscriber, &mEventToTopic)) {

        return;
    };

    mEventToTopic.at(type)->subscribers.insert(subscriber);
};

void EventSystem::unsubscribe(Uint32 type, Subscriber *subscriber) {
    if (!mEventToTopic.contains(type))
        return; // No topic with the given event type
    SDL_Log("EVENT TYPE ID: %d ", type);
    if (!isSubscriberInTopic(&type, subscriber, &mEventToTopic)) {
        return;
    };

    mEventToTopic.at(type)->subscribers.erase(subscriber);
};

void EventSystem::publishToTopic(SDL_Event *event) {
    if (!mEventToTopic.contains(event->type)) {
        // Discard event, no listeners.
        return;
    }

    for (Subscriber *subscriber : mEventToTopic.at(event->type)->subscribers) {
        subscriber->onNotify(event);
    };
};

}; // namespace HLX
