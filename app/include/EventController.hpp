#ifndef EVENTCONTROLLER_HPP
#define EVENTCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>

namespace app {

enum Actions {
    PRESS,
    MOVE,
    SCROLL,
    NOTHING,
    ACTIONS_COUNT
};

enum EventA {
    MOUSE = ACTIONS_COUNT,
    KEY,
    NOTHING_A,
    EVENTA_COUNT
};

enum EventB {
    CAMERA_POSITION = EVENTA_COUNT,
    CAMERA_ROTATION,
    CAMERA_ZOOM,
    CAMERA_SPEED_INCREASED,
    CAMERA_SPEED_STANDARD,
    LIGHTS_TOGGLE_POINT,
    LIGHTS_TOGGLE_DIR,
    LIGHTS_TOGGLE_SPOTLIGHT,
    MODEL_POSITION,
    MODEL_TOGGLE_NORMALS,
    GUI_TOGGLE,
    NOTHING_B,
    EVENTB_COUNT
};

const static size_t ACTIONS_EVENTS_COUNT = EVENTB_COUNT;

class Action {
public:
    Actions action;
    float actionTime;
    EventA eventA;
    float eventTime;
    EventB eventB;
};

class EventController : public engine::core::Controller {
public:

    static void notify(Action &&action);

    static void instaLog(Action &&action);

    static std::string_view actionsString(Actions &action);

    static std::string_view eventAString(EventA &eventA);

    static std::string_view eventBString(EventB &eventB);


private:

    static void logActions();

    static void logAction(Action &action);

    void initialize() override;

    void poll_events() override;

    std::string_view name() const override;

};
}

#endif //EVENTCONTROLLER_HPP
