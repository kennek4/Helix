#pragma once

#include "HLX_Subscriber.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_stdinc.h>
#include <map>
#include <set>

namespace HLX {
typedef struct Topic {
    Uint32 type;
    std::set<Subscriber *> subscribers{};
    Topic(Uint32 type) : type(type) {};

} Topic;

class EventSystem {
  public:
    static EventSystem &getInstance() {
        static EventSystem instance;
        return instance;
    };

    void subscribe(Uint32 type, Subscriber *subscriber);
    void unsubscribe(Uint32 type, Subscriber *subscriber);
    void publishToTopic(SDL_Event *event);

    inline std::map<Uint32, Topic *> &getEventToTopicMap() {
        return mEventToTopic;
    };

  private:
    EventSystem() = default;
    ~EventSystem();
    EventSystem(const EventSystem &eventSystem) = delete;
    EventSystem operator=(const EventSystem &eventSystem) = delete;

    std::map<Uint32, Topic *> mEventToTopic;
};
}; // namespace HLX
