#include <EventController.hpp>

namespace app {

void initialize_action_maps();

static std::array<std::string_view, ACTIONS_EVENTS_COUNT> ActionToString;
static std::vector<Action> actions;
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
            instaLog(Action(Actions::NOTHING, t, EventA::NOTHING_A, 0, EventB::NOTHING_B));
        }
//        logActions();
    }
}

void EventController::notify(Action &&action) {
    actions.emplace_back(action);
}

void EventController::logAction(Action &action) {
    t = 0.0f;
    spdlog::info("{0} ---> after {1:.2f}s ---> {2} ---> after {3:.2f}s ---> {4}",
                 actionsString(action.action), action.actionTime, eventAString(action.eventA), action.eventTime,
                 eventBString(action.eventB));
}

void EventController::instaLog(Action &&action) {
    t = 0.0f;
    spdlog::info("{0} ---> after {1:.2f}s ---> {2} ---> after {3:.2f}s ---> {4}",
                 actionsString(action.action), action.actionTime, eventAString(action.eventA), action.eventTime,
                 eventBString(action.eventB));
}

void EventController::logActions() {
    if (!actions.empty()) {
        for (auto &action: actions) {
            logAction(action);
        }
        actions.clear();
    }
}

std::string_view EventController::name() const {
    return "app::EventController";
}

std::string_view EventController::actionsString(Actions &action) {
    return ActionToString[action];
}

std::string_view EventController::eventAString(EventA &eventA) {
    return ActionToString[eventA];
}

std::string_view EventController::eventBString(EventB &eventB) {
    return ActionToString[eventB];
}

}