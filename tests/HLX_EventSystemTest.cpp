#include <HLX_EventSystem.h>
#include <HLX_Subscriber.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include <gtest/gtest.h>
#include <map>
#include <print>

class A : public HLX::Subscriber {
  public:
    int previousX{0};
    int previousy{0};
    int currentX{0};
    int currentY{0};

    A() {
        HLX::EventSystem::getInstance().subscribe(SDL_EVENT_MOUSE_MOTION, this);
    };

    bool onNotify(SDL_Event *event) override {
        SDL_Log("Recieved SDL_EVENT_MOUSE_MOTION event!");
        previousX = currentX;
        previousy = currentY;
        currentX = event->motion.x;
        currentY = event->motion.y;
        return true;
    };
};

class B : public HLX::Subscriber {
  public:
    SDL_Scancode prevScanCode{SDL_SCANCODE_A};
    SDL_Scancode currScanCode{SDL_SCANCODE_A};
    B() {
        HLX::EventSystem::getInstance().subscribe(SDL_EVENT_KEY_DOWN, this);
    };

    bool onNotify(SDL_Event *event) override {
        SDL_Log("Recieved SDL_EVENT_KEY_DOWN event!");
        prevScanCode = currScanCode;
        currScanCode = event->key.scancode;
        return true;
    };
};

class HelixEventSystemTest : public testing::Test {
  protected:
    HelixEventSystemTest() {

        HLX::EventSystem::getInstance();

        SDL_Init(SDL_INIT_EVENTS);
        a = new A();
        b = new B();

        SDL_zero(mouseEvent);
        mouseEvent.type = SDL_EVENT_MOUSE_MOTION;
        mouseEvent.motion.x = 1600;
        mouseEvent.motion.y = 900;

        SDL_zero(keyEvent);
        keyEvent.type = SDL_EVENT_KEY_DOWN;
        keyEvent.key.scancode = SDL_SCANCODE_Z;

        SDL_zero(windowEvent);
        windowEvent.type = SDL_EVENT_WINDOW_RESIZED;
    };

    ~HelixEventSystemTest() override {
        delete a;
        delete b;
        SDL_Quit();
    };

    void TearDown() override {
        delete a;
        delete b;

        a = new A();
        b = new B();
    };

    std::map<Uint32, HLX::Topic *> &map =
        HLX::EventSystem::getInstance().getEventToTopicMap();

    A *a;
    B *b;

    SDL_Event mouseEvent;
    SDL_Event keyEvent;
    SDL_Event windowEvent;
};

TEST_F(HelixEventSystemTest, PublishAsserts) {

    std::println("Publishing mouseEvent...");
    EXPECT_EQ(a->previousX, 0);
    EXPECT_EQ(a->previousy, 0);
    EXPECT_EQ(a->currentX, 0);
    EXPECT_EQ(a->currentY, 0);

    HLX::EventSystem::getInstance().publishToTopic(&mouseEvent);
    map = HLX::EventSystem::getInstance().getEventToTopicMap();

    EXPECT_TRUE(map.contains(mouseEvent.type));
    EXPECT_EQ(a->previousX, 0);
    EXPECT_EQ(a->previousy, 0);
    EXPECT_EQ(a->currentX, 1600);
    EXPECT_EQ(a->currentY, 900);

    mouseEvent.motion.x = 1920;
    mouseEvent.motion.y = 1080;

    HLX::EventSystem::getInstance().publishToTopic(&mouseEvent);
    map = HLX::EventSystem::getInstance().getEventToTopicMap();

    EXPECT_EQ(a->previousX, 1600);
    EXPECT_EQ(a->previousy, 900);
    EXPECT_EQ(a->currentX, 1920);
    EXPECT_EQ(a->currentY, 1080);

    std::println("Publishing keyEvent...");
    EXPECT_EQ(b->prevScanCode, SDL_SCANCODE_A);
    EXPECT_EQ(b->currScanCode, SDL_SCANCODE_A);

    HLX::EventSystem::getInstance().publishToTopic(&keyEvent);
    map = HLX::EventSystem::getInstance().getEventToTopicMap();

    EXPECT_TRUE(map.contains(mouseEvent.type));
    EXPECT_EQ(b->prevScanCode, SDL_SCANCODE_A);
    EXPECT_EQ(b->currScanCode, SDL_SCANCODE_Z);
};
