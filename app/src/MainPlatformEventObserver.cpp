#include <MainPlatformEventObserver.hpp>
#include <GUIController.hpp>
#include <EventController.hpp>
#include <MainController.hpp>

namespace app {

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto eventController = engine::core::Controller::get<EventController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float actionStart = platform->get_glfw_time();
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        float actionEnd = platform->get_glfw_time();
        float eventStart = platform->get_glfw_time();
        camera->rotate_camera(position.dx, position.dy);
        float eventEnd = platform->get_glfw_time();
        eventController->insta_log(Action(Actions::MOVE, actionEnd - actionStart, EventA::MOUSE,
                                          eventEnd - eventStart, EventB::CAMERA_ROTATION));
    }

}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    auto eventController = engine::core::Controller::get<EventController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float actionStart = platform->get_glfw_time();
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        float actionEnd = platform->get_glfw_time();
        float eventStart = platform->get_glfw_time();
        camera->zoom(position.scroll);
        float eventEnd = platform->get_glfw_time();
        eventController->insta_log(Action(Actions::SCROLL, actionEnd - actionStart, EventA::MOUSE,
                                          eventEnd - eventStart, EventB::CAMERA_ZOOM));
    }
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    auto eventController = engine::core::Controller::get<EventController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float actionStart = platform->get_glfw_time();
    auto guiController = engine::core::Controller::get<GUIController>();
    auto &controls = MainController::get_key_controls();
    if (!guiController->is_enabled()) {
        float actionEnd = platform->get_glfw_time();
        float eventStart = platform->get_glfw_time();
        if (controls.find(key.id()) == end(controls)) {
            spdlog::info("{0} is not supported.", key.name());
            float eventEnd = platform->get_glfw_time();
            eventController->insta_log(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY,
                                              eventEnd - eventStart, EventB::NOTHING_B));
        }
    }
}

}
