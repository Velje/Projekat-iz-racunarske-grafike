#include <MainController.hpp>
#include <GUIController.hpp>
#include <MainPlatformEventObserver.hpp>
#include <LightController.hpp>
#include <EventController.hpp>

namespace app {

static std::unordered_map<engine::platform::KeyId, engine::graphics::Camera::Movement> KeyIdToCameraMovement;
static std::vector<glm::mat4> modelMatrices;

const std::unordered_map<engine::platform::KeyId, engine::graphics::Camera::Movement> &MainController::getKeyIdToCameraMovement() {
    return KeyIdToCameraMovement;
}

static uint32_t gBuffer, rboDepth;
static std::array<uint32_t, 3> attachments, textureIDs;

void initialize_keyid_maps();

void MainController::initialize() {
    auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    KeyIdToCameraMovement.rehash(engine::graphics::Camera::Movement::MOVEMENT_COUNT);
    initialize_keyid_maps();
    engine::graphics::OpenGL::enable_depth_testing();
    textureIDs = engine::graphics::OpenGL::generateGbuffer(gBuffer, rboDepth, attachments, window->width(),
                                                           window->height());
//    engine::graphics::OpenGL::enable_antialiasing();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f));
    modelMatrices.push_back(model);
    modelMatrices.push_back(glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f)));
    modelMatrices.push_back(glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f)));
    modelMatrices.push_back(glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f)));
    modelMatrices.push_back(glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f)));
//    for (uint32_t i = 0; i < 10; i++) {
//        modelMatrices.push_back(glm::translate(model, glm::vec3(2 * (i + 1), 8.0f, 2 * (i + 1))));
//    }
//    for (uint32_t i = 0; i < 10; i++) {
//        modelMatrices.push_back(glm::translate(model, glm::vec3(0, 2 * (i + 1), 0)));
//    }
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
                camera->MovementSpeed = 50.0f;
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

void MainController::drawTerrain() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *brown_mud = resources->model("brown_mud");
    engine::resources::Shader *shader = resources->shader("terrain");
    engine::resources::Shader *lightShader = resources->shader("lightPass");

    engine::graphics::OpenGL::bindFrameBuffer(gBuffer);
    engine::graphics::OpenGL::clear_buffers();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader->set_mat4("model", model);
    shader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    brown_mud->draw(shader);
    engine::graphics::OpenGL::bindFrameBuffer(0);

    lightShader->use();
    engine::graphics::OpenGL::activateGbuffertextures(textureIDs);
    lightShader->set_int("gPosition", 0);
    lightShader->set_int("gNormal", 1);
    lightShader->set_int("gAlbedoSpec", 2);
    lightShader->set_vec3("viewPos", graphics->camera()
                                             ->Position);
    auto light = engine::core::Controller::get<LightController>();
    auto pointLights = light->getPointLights();
    auto dirLights = light->getDirectionalLights();
    auto spotLights = light->getSpotLights();
    light->setShaderPointLights(lightShader, "light", pointLights);
    light->setShaderDirLights(lightShader, "dirLight", dirLights);
    light->setShaderSpotLights(lightShader, "spotLight", spotLights);
    engine::graphics::OpenGL::renderScreen();

}

void MainController::drawUFO() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *ufo_obj = resources->model("UFO_obj");
    engine::resources::Shader *shader = resources->shader("ufo");
    engine::resources::Shader *lightShader = resources->shader("lightPass");
    engine::graphics::OpenGL::bindFrameBuffer(gBuffer);
    engine::graphics::OpenGL::clear_buffers();
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f));
    shader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    ufo_obj->prepareInstancing(modelMatrices, 5);
    ufo_obj->drawInstances(shader, 5);
    engine::graphics::OpenGL::bindFrameBuffer(0);

    lightShader->use();
    lightShader->set_int("gPosition", 0);
    lightShader->set_int("gNormal", 1);
    lightShader->set_int("gAlbedoSpec", 2);
    lightShader->set_vec3("viewPos", graphics->camera()
                                             ->Position);
    engine::graphics::OpenGL::activateGbuffertextures(textureIDs);
    auto light = engine::core::Controller::get<LightController>();
    auto pointLights = light->getPointLights();
    auto dirLights = light->getDirectionalLights();
    auto spotLights = light->getSpotLights();
    light->setShaderPointLights(lightShader, "light", pointLights);
    light->setShaderDirLights(lightShader, "dirLight", dirLights);
    light->setShaderSpotLights(lightShader, "spotLight", spotLights);
    engine::graphics::OpenGL::renderScreen();
    auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
    engine::graphics::OpenGL::writeToDefaultFramebuffer(gBuffer, window->width(), window->height());
}

void MainController::drawSkybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("skybox220");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
//    drawTerrain();
    drawUFO();
    drawSkybox();
//    drawAliens();
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

std::string_view MainController::name() const {
    return "app::MainController";
}

}
