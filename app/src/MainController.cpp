#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
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

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    if (platform->key(engine::platform::KeyId::KEY_W)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, platform->dt());
    }
    if (platform->key(engine::platform::KeyId::KEY_S)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, platform->dt());
    }
    if (platform->key(engine::platform::KeyId::KEY_A)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, platform->dt());
    }
    if (platform->key(engine::platform::KeyId::KEY_D)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, platform->dt());
    }
}

void MainController::update() {
    update_camera();
}

void MainController::drawBackpack() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *backpack = resources->model("backpack");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);
    backpack->draw(shader);
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    drawBackpack();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

std::string_view MainController::name() const {
    return "app::MainController";
}


}
