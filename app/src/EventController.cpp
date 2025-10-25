#include <EventController.hpp>

namespace app {

void initialize_action_maps();

static std::array<std::string_view, ACTIONS_EVENTS_COUNT> g_action_to_string;
static float t = 0.0f;

void EventController::initialize() {
    initialize_action_maps();
}

void initialize_action_maps() {
    // @formatter:off
    #include "action_to_string.include"
    // @formatter:on
}

void EventController::poll_events() {
    if (is_enabled()) {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        t += platform->dt();
        if (t >= 10.0f) {
            insta_log(Action(Actions::NOTHING, t, EventA::NOTHING_A, 0, EventB::NOTHING_B));
        }
//        log_actions();
    }
}

void EventController::notify(Action &&action) {
    m_actions.emplace_back(action);
}

void EventController::log_action(Action &action) {
    t = 0.0f;
    spdlog::info("{0} ---> after {1:.2f}s ---> {2} ---> after {3:.2f}s ---> {4}",
                 actions_string(action.action), action.action_time, event_a_string(action.event_a), action.event_time,
                 event_b_string(action.event_b));
}

void EventController::insta_log(Action &&action) {
    t = 0.0f;
    spdlog::info("{0} ---> after {1:.2f}s ---> {2} ---> after {3:.2f}s ---> {4}",
                 actions_string(action.action), action.action_time, event_a_string(action.event_a), action.event_time,
                 event_b_string(action.event_b));
}

void EventController::log_actions() {
    if (!m_actions.empty()) {
        for (auto &action: m_actions) {
            log_action(action);
        }
        m_actions.clear();
    }
}

std::string_view EventController::name() const {
    return "app::EventController";
}

std::string_view EventController::actions_string(Actions &action) {
    return g_action_to_string[action];
}

std::string_view EventController::event_a_string(EventA &event_a) {
    return g_action_to_string[event_a];
}

std::string_view EventController::event_b_string(EventB &event_b) {
    return g_action_to_string[event_b];
}

}