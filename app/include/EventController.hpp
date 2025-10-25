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

struct Action {
    Actions action;
    float action_time;
    EventA event_a;
    float event_time;
    EventB event_b;
};

class MainController;

class EventController : public engine::core::Controller {
public:

    void notify(Action &&action);

    void insta_log(Action &&action);

    static std::string_view actions_string(Actions &action);

    static std::string_view event_a_string(EventA &event_a);

    static std::string_view event_b_string(EventB &event_b);


private:

    MainController *m_main_controller;

    std::vector<Action> m_actions;

    void log_actions();

    void log_action(Action &action);

    void initialize() override;

    void poll_events() override;

    std::string_view name() const override;

};
}

#endif //EVENTCONTROLLER_HPP
