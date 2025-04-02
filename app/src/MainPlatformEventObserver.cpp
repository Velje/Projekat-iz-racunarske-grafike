#include <MainPlatformEventObserver.hpp>
#include <GUIController.hpp>
#include <EventController.hpp>
#include <MainController.hpp>

namespace app {
void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float actionStart = platform->getGlfwTime();
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        float actionEnd = platform->getGlfwTime();
        float eventStart = platform->getGlfwTime();
        camera->rotate_camera(position.dx, position.dy);
        float eventEnd = platform->getGlfwTime();
        auto eventController = engine::core::Controller::get<EventController>();
        eventController->notify(Action(Actions::MOVE, actionEnd - actionStart, EventA::MOUSE,
                                       eventEnd - eventStart, EventB::CAMERA_ROTATION));
    }

}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float actionStart = platform->getGlfwTime();
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        float actionEnd = platform->getGlfwTime();
        float eventStart = platform->getGlfwTime();
        camera->zoom(position.scroll);
        float eventEnd = platform->getGlfwTime();
        auto eventController = engine::core::Controller::get<EventController>();
        eventController->notify(Action(Actions::SCROLL, actionEnd - actionStart, EventA::MOUSE,
                                       eventEnd - eventStart, EventB::CAMERA_ZOOM));
    }
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    auto guiController = engine::core::Controller::get<GUIController>();
    if (!guiController->is_enabled()) {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto keyIdToCameraMovement = engine::core::Controller::get<MainController>()->getKeyIdToCameraMovement();
        if (keyIdToCameraMovement.contains(key.id())) {
        }
    }
}

}
