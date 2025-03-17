#include <MainController.hpp>
#include <MainPlatformEventObserver.hpp>
#include <GUIController.hpp>
#include <spdlog/spdlog.h>

namespace app {

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto guiController = engine::core::Controller::get<GUIController>();
    if (guiController->is_enabled()) {
        return;
    }
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx, position.dy);
}

void MainController::initialize() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
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
    auto guiController = engine::core::Controller::get<GUIController>();
    if (guiController->is_enabled()) {
        return;
    }
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
    if (platform->key(engine::platform::KeyId::KEY_SPACE)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::UP, platform->dt());
    }
    if (platform->key(engine::platform::KeyId::KEY_LEFT_CONTROL)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::DOWN, platform->dt());
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

void MainController::drawSkybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("mountainSkybox");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    drawBackpack();
    drawSkybox();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

std::string_view MainController::name() const {
    return "app::MainController";
}


}
