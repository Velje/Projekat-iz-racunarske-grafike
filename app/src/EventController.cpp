#include <EventController.hpp>

namespace app {

void initialize_action_maps();

static std::array<std::string_view, ACTIONS_EVENTS_COUNT> ActionToString;
static std::vector<Action> actions;

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
        logActions();
    }
}

void EventController::notify(Action action) {
    actions.push_back(action);
}

void EventController::logAction(Action &action) {
    spdlog::info("{0} ---> after {1:.2f}s ---> {2} ---> after {3:.2f}s ---> {4}",
                 actionsString(action.action), action.eventTime, eventAString(action.eventA), action.actionTime,
                 eventBString(action.eventB));
}

void EventController::instaLog(Action action) {
    spdlog::info("{0} ---> after {1:.2f}s ---> {2} ---> after {3:.2f}s ---> {4}",
                 actionsString(action.action), action.eventTime, eventAString(action.eventA), action.actionTime,
                 eventBString(action.eventB));
}

void EventController::logActions() {
    if (actions.empty()) {
        auto eventController = engine::core::Controller::get<EventController>();
        eventController->instaLog(Action(Actions::NOTHING, 0, EventA::NOTHING_A, 0, EventB::NOTHING_B));
    } else {
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