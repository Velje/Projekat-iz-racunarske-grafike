#include <EventController.hpp>

namespace app {

void initialize_event_maps();

static std::array<std::string_view, totalEventStrings> EventToString;

void EventController::initialize() {
    initialize_event_maps();
}

void initialize_event_maps() {
    // @formatter:off
    #include "event_to_string.include"
    // @formatter:on
}

void EventController::poll_events() {
    if (is_enabled()) {
        logEvents();
    }
}

void EventController::notify(Event event) {
    events.push_back(event);
}

void EventController::logEvent(Event &event) {
    spdlog::info("{0} ---> after {1:.2f}s ---> {2} ---> after {3:.2f}s ---> {4}",
                 eventsString(event.event), event.eventTime, actionAString(event.actionA), event.actionTime,
                 actionBString(event.actionB));
}

void EventController::logEvents() {
    for (auto &event: events) {
        logEvent(event);
    }
    events.clear();
}

std::string_view EventController::name() const {
    return "app::EventController";
}

std::string_view EventController::eventsString(Events &event) {
    return EventToString[event];
}

std::string_view EventController::actionAString(ActionA &actionA) {
    uint32_t positionOffset = EVENT_COUNT;
    return EventToString[positionOffset + actionA];
}

std::string_view EventController::actionBString(ActionB &actionB) {
    uint32_t positionOffset = EVENT_COUNT + ACTIONA_COUNT;
    return EventToString[positionOffset + actionB];
}


}