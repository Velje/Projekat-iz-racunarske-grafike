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
        EventController::instaLog(Action(Actions::MOVE, actionEnd - actionStart, EventA::MOUSE,
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
        EventController::instaLog(Action(Actions::SCROLL, actionEnd - actionStart, EventA::MOUSE,
                                         eventEnd - eventStart, EventB::CAMERA_ZOOM));
    }
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float actionStart = platform->getGlfwTime();
    auto guiController = engine::core::Controller::get<GUIController>();
    auto &controls = MainController::getKeyControls();
    if (!guiController->is_enabled()) {
        float actionEnd = platform->getGlfwTime();
        float eventStart = platform->getGlfwTime();
        if (controls.find(key.id()) == end(controls)) {
            spdlog::info("{0} is not supported.", key.name());
            float eventEnd = platform->getGlfwTime();
            EventController::instaLog(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY,
                                             eventEnd - eventStart, EventB::NOTHING_B));
        }
    }
}

}
