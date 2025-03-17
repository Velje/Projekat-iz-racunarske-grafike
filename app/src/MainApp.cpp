#include <MainApp.hpp>

#include <spdlog/spdlog.h>
#include <MainController.hpp>

namespace app {
void MainApp::app_setup() {
    spdlog::info("App setup completed!");
    auto mainController = register_controller<app::MainController>();
    mainController->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}
}