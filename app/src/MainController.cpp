#include <MainController.hpp>
#include <GUIController.hpp>
#include <MainPlatformEventObserver.hpp>
#include <LightController.hpp>
#include <EventController.hpp>

namespace app {

static std::unordered_map<engine::platform::KeyId, engine::graphics::Camera::Movement> KeyIdToCameraMovement;

const std::unordered_map<engine::platform::KeyId, engine::graphics::Camera::Movement> &MainController::getKeyIdToCameraMovement() {
    return KeyIdToCameraMovement;
}

static std::vector<engine::resources::Vertex> vertices;
static std::vector<uint32_t> indices;
static std::vector<engine::resources::Texture *> textures;
static std::unique_ptr<engine::resources::Mesh> mesh;

void initialize_keyid_maps();

void MainController::initialize() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    KeyIdToCameraMovement.rehash(engine::graphics::Camera::Movement::MOVEMENT_COUNT);
    initialize_keyid_maps();
    vertices = {
            engine::resources::Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec2(1.0f, 1.0f)),  // top right
            engine::resources::Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec2(1.0f, 0.0f)), // bottom right
            engine::resources::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec2(0.0f, 0.0f)),  // bottom left
            engine::resources::Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec2(0.0f, 1.0f)
            )  // top left
    };
    indices = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
    };
    auto texture0 = resources->texture("wall", "", engine::resources::TextureType::Diffuse, true);
    textures = {texture0};
    mesh = std::make_unique<engine::resources::Mesh>(vertices, indices, textures);
    engine::graphics::OpenGL::enable_depth_testing();
    engine::graphics::OpenGL::enable_antialiasing();
}

void initialize_keyid_maps() {
    // @formatter:off
    #include "keyid_to_camera_movement.include"
    // @formatter:on
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
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto eventController = engine::core::Controller::get<EventController>();
    auto camera = graphics->camera();
    auto deltaTime = platform->dt();
    for (auto &pair: KeyIdToCameraMovement) {
        if (platform->key(pair.first)
                    .is_down()) {
            float actionStart = platform->getGlfwTime();
            float actionEnd = platform->getGlfwTime();
            float eventStart = platform->getGlfwTime();
            float eventEnd;
            if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT)
                        .is_down()) {
                camera->MovementSpeed = 7.0f;
                eventEnd = platform->getGlfwTime();
                eventController->notify(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEYBOARD, eventEnd - eventStart,
                               EventB::CAMERA_SPEED_INCREASED));
            } else {
                camera->MovementSpeed = 2.5f;
                eventEnd = platform->getGlfwTime();
                eventController->notify(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEYBOARD, eventEnd - eventStart,
                               EventB::CAMERA_SPEED_STANDARD));
            }
            eventStart = platform->getGlfwTime();
            camera->move_camera(pair.second, deltaTime);
            eventEnd = platform->getGlfwTime();
            eventController->notify(
                    Action(Actions::PRESS, actionEnd - actionStart, EventA::KEYBOARD, eventEnd - eventStart,
                           EventB::CAMERA_POSITION));
        }
    }
}

void MainController::update() {
    auto guiController = engine::core::Controller::get<GUIController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (guiController->is_enabled()) {
        platform->set_enable_cursor(true);
        return;
    } else {
        update_camera();
        platform->set_enable_cursor(false);
    }
}

void MainController::drawBackpack() {
    auto light = engine::core::Controller::get<LightController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *backpack = resources->model("backpack");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
    shader->set_mat4("model", model);
    shader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    shader->set_vec3("viewPos", graphics->camera()
                                        ->Position);
    auto pointLights = light->getPointLights();
    auto dirLights = light->getDirectionalLights();
    light->setShaderPointLights(shader, "light", pointLights);
    light->setShaderDirLights(shader, "dirLight", dirLights);
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
    auto light = engine::core::Controller::get<LightController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
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
    shader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    shader->set_vec3("viewPos", graphics->camera()
                                        ->Position);
    auto dirLights = light->getDirectionalLights();
    light->setShaderDirLights(shader, "dirLight", dirLights);
    mesh->draw(shader);
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
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

std::string_view MainController::name() const {
    return "app::MainController";
}


}
