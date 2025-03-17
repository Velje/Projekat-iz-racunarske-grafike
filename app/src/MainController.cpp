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

void MainController::drawBackpack() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Model *model = resources->model("backpack");
    engine::resources::Shader *shader = resources->shader("basic");
    model->draw(shader);
}

void MainController::draw() {
    drawBackpack();
}

std::string_view MainController::name() const {
    return "app::MainController";
}
}
