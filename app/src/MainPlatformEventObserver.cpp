#include <MainPlatformEventObserver.hpp>
#include <GUIController.hpp>

namespace app {
void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto guiController = engine::core::Controller::get<GUIController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        camera->rotate_camera(position.dx, position.dy);
    }
}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    auto guiController = engine::core::Controller::get<GUIController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    if (!guiController->is_enabled()) {
        camera->zoom(position.scroll);
    }
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
}
}
