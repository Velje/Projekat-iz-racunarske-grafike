#include <MainPlatformEventObserver.hpp>
#include <GUIController.hpp>
#include <EventController.hpp>

namespace app {
void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    app::Event event;
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float eventStart = platform->getGlfwTime();
    event.event = app::Events::EVENT_MOUSE;
    event.actionA = app::ActionA::MOVE;
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        float eventEnd = platform->getGlfwTime();
        float actionStart = platform->getGlfwTime();
        camera->rotate_camera(position.dx, position.dy);
        float actionEnd = platform->getGlfwTime();
        event.actionB = app::ActionB::CAMERA_ROTATION;
        event.actionTime = actionEnd - actionStart;
        event.eventTime = eventEnd - eventStart;
        auto eventController = engine::core::Controller::get<EventController>();
        eventController->notify(event);
    }

}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    app::Event event;
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float eventStart = platform->getGlfwTime();
    event.event = Events::EVENT_MOUSE;
    event.actionA = ActionA::SCROLL;
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        float eventEnd = platform->getGlfwTime();
        float actionStart = platform->getGlfwTime();
        camera->zoom(position.scroll);
        float actionEnd = platform->getGlfwTime();
        event.actionB = app::ActionB::CAMERA_ZOOM;
        event.actionTime = actionEnd - actionStart;
        event.eventTime = eventEnd - eventStart;
        auto eventController = engine::core::Controller::get<EventController>();
        eventController->notify(event);
    }
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    app::Event event;
    auto eventController = engine::core::Controller::get<EventController>();
    event.event = Events::EVENT_KEYBOARD;
    event.actionA = ActionA::PRESS;
}

}
