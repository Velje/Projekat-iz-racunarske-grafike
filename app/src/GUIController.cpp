#include <GUIController.hpp>
#include <EventController.hpp>

namespace app {

void GUIController::initialize() {
    set_enable(false);
}

void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto eventController = engine::core::Controller::get<EventController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed
            ) {
        eventController->set_enable(!eventController->is_enabled());
        set_enable(!is_enabled());
    }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = graphics->camera();
    graphics->begin_gui();
    ImGui::Begin("Camera info");
    ImGui::Text("Camera position: (%f, %f, %f)", camera->Position
                                                       .x, camera->Position
                                                                 .y, camera->Position
                                                                           .z);
    ImGui::Text("Mouse position: %f %f", platform->mouse()
                                                 .x, platform->mouse()
                                                             .y);
    auto deltaTime = platform->dt();
    ImGui::Text("Frames per second: %f", 1 / deltaTime);
    ImGui::Text("Frame difference: %fms", deltaTime * 1000);

    ImGui::End();
    graphics->end_gui();
}

std::string_view GUIController::name() const {
    return "app::GUIController";
}
}


