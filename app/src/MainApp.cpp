#include <MainApp.hpp>
#include <MainController.hpp>
#include <GUIController.hpp>
#include <EventController.hpp>

namespace app {
void MainApp::app_setup() {
    auto mainController = register_controller<app::MainController>();
    auto guiController = register_controller<app::GUIController>();
    auto eventController = register_controller<app::EventController>();
    mainController->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    mainController->before(guiController);
    guiController->before(eventController);
    spdlog::info("App setup completed!");
}
}