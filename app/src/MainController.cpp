#include <MainController.hpp>
#include <GUIController.hpp>
#include <MainPlatformEventObserver.hpp>
#include <spdlog/spdlog.h>

namespace app {

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

void MainController::poll_events() {
    auto guiController = engine::core::Controller::get<GUIController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (guiController->is_enabled()) {
        platform->set_enable_cursor(true);
        return;
    } else {
        platform->set_enable_cursor(false);
    }
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    auto deltaTime = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_W)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, deltaTime);
    }
    if (platform->key(engine::platform::KeyId::KEY_S)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, deltaTime);
    }
    if (platform->key(engine::platform::KeyId::KEY_A)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, deltaTime);
    }
    if (platform->key(engine::platform::KeyId::KEY_D)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, deltaTime);
    }
    if (platform->key(engine::platform::KeyId::KEY_UP)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::UP, deltaTime);
    }
    if (platform->key(engine::platform::KeyId::KEY_DOWN)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::DOWN, deltaTime);
    }
}

void MainController::update() {
    auto guiController = engine::core::Controller::get<GUIController>();
    if (!guiController->is_enabled()) {
        update_camera();
    }
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

void MainController::drawFloor() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto texture0 = resources->texture("wall", "", engine::resources::TextureType::Regular, true);
    std::vector<engine::resources::Vertex> vertices = {
            engine::resources::Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f),
                                      glm::vec3(0.0f)),  // top right
            engine::resources::Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f),
                                      glm::vec3(0.0f)), // bottom right
            engine::resources::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f),
                                      glm::vec3(0.0f)),  // bottom left
            engine::resources::Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f),
                                      glm::vec3(0.0f))  // top left
    };
    std::vector<uint32_t> indices = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
    };
    std::vector<engine::resources::Texture *> textures = {texture0};
    std::unique_ptr Mesh = std::make_unique<engine::resources::Mesh>(vertices, indices, textures);
    auto shader = resources->shader("textureShader");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f));
    shader->set_mat4("model", model);
    Mesh->draw(shader);
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    drawBackpack();
    drawSkybox();
    drawFloor();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

std::string_view MainController::name() const {
    return "app::MainController";
}


}
