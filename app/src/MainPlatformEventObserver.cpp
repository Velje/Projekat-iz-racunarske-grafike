#include <MainPlatformEventObserver.hpp>
#include <GUIController.hpp>
#include <EventController.hpp>
#include <MainController.hpp>

namespace app {
void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float eventStart = platform->getGlfwTime();
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        float eventEnd = platform->getGlfwTime();
        float actionStart = platform->getGlfwTime();
        camera->rotate_camera(position.dx, position.dy);
        float actionEnd = platform->getGlfwTime();
        auto eventController = engine::core::Controller::get<EventController>();
        eventController->notify(Event(Events::EVENT_MOUSE, eventEnd - eventStart,
                                      ActionA::MOVE, actionEnd - actionStart,
                                      ActionB::CAMERA_ROTATION));
    }

}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float eventStart = platform->getGlfwTime();
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        float eventEnd = platform->getGlfwTime();
        float actionStart = platform->getGlfwTime();
        camera->zoom(position.scroll);
        float actionEnd = platform->getGlfwTime();
        auto eventController = engine::core::Controller::get<EventController>();
        eventController->notify(
                Event(Events::EVENT_MOUSE, eventEnd - eventStart, ActionA::SCROLL, actionEnd - actionStart,
                      ActionB::CAMERA_ZOOM));
    }
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float eventStart = platform->getGlfwTime();
    float eventEnd = platform->getGlfwTime();
    float actionStart = platform->getGlfwTime();
    float actionEnd = platform->getGlfwTime();
    auto eventController = engine::core::Controller::get<EventController>();
    eventController->notify(Event(Events::EVENT_KEYBOARD, eventEnd - eventStart, ActionA::PRESS,
                                  actionEnd - actionStart, ActionB::CAMERA_POSITION));
}

}
