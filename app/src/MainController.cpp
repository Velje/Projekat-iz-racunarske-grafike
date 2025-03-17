#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MainController::initialize() {
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .is_down()) {
        spdlog::info("ESC pressed. Exiting...");
        return false;
    }
    return true;
}

std::string_view MainController::name() const {
    return "app::MainController";
}
}
