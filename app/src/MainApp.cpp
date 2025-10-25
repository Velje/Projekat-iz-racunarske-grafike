#include <MainApp.hpp>
#include <MainController.hpp>
#include <GUIController.hpp>
#include <EventController.hpp>
#include <LightController.hpp>

namespace app {
void MainApp::app_setup() {
    auto mainController = register_controller<MainController>();
    auto guiController = register_controller<GUIController>();
    auto eventController = register_controller<EventController>();
    auto lightController = register_controller<LightController>();
    lightController->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    lightController->before(mainController);
    mainController->before(guiController);
    guiController->before(eventController);
    spdlog::info("App setup completed!");
}
}