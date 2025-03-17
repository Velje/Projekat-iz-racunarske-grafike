#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MainController::initialize() {
    spdlog::info("MainController initialized");
}

std::string_view MainController::name() const {
    return "app::MainController";
}
}
