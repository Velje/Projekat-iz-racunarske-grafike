#ifndef EVENTCONTROLLER_HPP
#define EVENTCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <spdlog/spdlog.h>

namespace app {

enum Events {
    EVENT_MOUSE,
    EVENT_KEYBOARD,
    EVENT_NOTHING,
    EVENT_COUNT
};

enum ActionA {
    PRESS,
    MOVE,
    SCROLL,
    NOTHING_A,
    ACTIONA_COUNT
};

enum ActionB {
    CAMERA_POSITION,
    CAMERA_ROTATION,
    CAMERA_ZOOM,
    NOTHING_B,
    ACTIONB_COUNT
};

const static size_t EVENTS_ACTIONS_COUNT = static_cast <size_t> (EVENT_COUNT) +
                                           static_cast <size_t> (ACTIONA_COUNT) +
                                           static_cast <size_t> (ACTIONB_COUNT);

class Event {
public:
    Events event;
    float eventTime;
    ActionA actionA;
    float actionTime;
    ActionB actionB;
};

class EventController : public engine::core::Controller {
public:

    void notify(Event event);

    static std::string_view eventsString(Events &event);

    static std::string_view actionBString(ActionB &actionB);

    static std::string_view actionAString(ActionA &actionA);

private:

    static void logEvent(Event &event);

    static void logEvents();

    void initialize() override;

    void poll_events() override;

    std::string_view name() const override;

};
}

#endif //EVENTCONTROLLER_HPP
