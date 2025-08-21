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

static uint32_t gBuffer, rboDepth;
static std::array<uint32_t, 3> attachments, textureIDs;
static std::vector<glm::mat4> ufoMatrices;
static float t = 0.0;

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
    std::vector<std::pair<float, float>> ufos;
    const uint32_t gridSize2 = 30;
    const float spacing2 = 200.0f / gridSize2;

    for (uint32_t row = 0; row < gridSize2; ++row) {
        for (uint32_t col = 0; col < gridSize2; ++col) {
            float x = -100.0f + col * spacing2 + spacing2 / 2.0f;
            float z = -100.0f + row * spacing2 + spacing2 / 2.0f;
            ufos.emplace_back(x, z);
        }
    }
    for (uint64_t i = 0; i < 900; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(ufos[i].first, 7.0f, 200.0f + ufos[i].second));
        model = glm::scale(model, glm::vec3(0.1f));
        ufoMatrices.push_back(model);
    }
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
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_P)
                .state() == engine::platform::Key::State::JustPressed) {
        for (auto &pointLight: LightController::getPointLights()) {
            LightController::togglePoint(pointLight);
        }
    }
    if (platform->key(engine::platform::KeyId::KEY_T)
                .state() == engine::platform::Key::State::JustPressed) {
        for (auto &dirLight: LightController::getDirectionalLights()) {
            LightController::toggleDirectional(dirLight);
        }
        for (auto &spotLight: LightController::getSpotLights()) {
            LightController::toggleSpot(spotLight);
        }
    }
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
        platform->set_enable_cursor(false);
        update_camera();
    }
}

void MainController::deferredRender() {
    geometryPass();
    lightPass();
    auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
    engine::graphics::OpenGL::writeToDefaultFramebuffer(gBuffer, window->width(), window->height());
}

void MainController::geometryPass() {
    engine::graphics::OpenGL::bindFrameBuffer(gBuffer);
    engine::graphics::OpenGL::clear_buffers();
    drawTerrain();
    engine::graphics::OpenGL::enable_backCulling();
    drawUFO();
    drawUFO2();
    drawEarth();
//    drawPlatform();
    engine::graphics::OpenGL::bindFrameBuffer(0);
}

void MainController::lightPass() {
    auto light = engine::core::Controller::get<LightController>();
    light->updateLights();
    engine::graphics::OpenGL::activateGbuffertextures(textureIDs);
    engine::graphics::OpenGL::renderScreen();
}

void MainController::drawTerrain() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *brown_mud = resources->model("brown_mud");
    engine::resources::Shader *defaultShader = resources->shader("default");
    defaultShader->use();
    defaultShader->set_mat4("projection", graphics->projection_matrix());
    defaultShader->set_mat4("view", graphics->camera()
                                            ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 200.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    defaultShader->set_mat4("model", model);
    defaultShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    brown_mud->draw(defaultShader);
}

void MainController::drawEarth() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *earth = resources->model("Earth");
    engine::resources::Shader *defaultShader = resources->shader("default");
    defaultShader->use();
    defaultShader->set_mat4("projection", graphics->projection_matrix());
    defaultShader->set_mat4("view", graphics->camera()
                                            ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -700.0f));
    model = glm::scale(model, glm::vec3(50.0f));
    defaultShader->set_mat4("model", model);
    defaultShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    earth->draw(defaultShader);
}

void MainController::drawUFO() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    engine::resources::Model *ufo_obj = resources->model("UFO");
    engine::resources::Shader *defaultShader = resources->shader("default");
    defaultShader->use();
    defaultShader->set_mat4("projection", graphics->projection_matrix());
    defaultShader->set_mat4("view", graphics->camera()
                                            ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 25.0f, 200.0f));
    model = glm::scale(model, glm::vec3(20.0f));
    float angle = t + 2.0f * M_PI / NR_SPOT_LIGHTS;
    model = glm::rotate(model, angle,
                        glm::vec3(0.0f, -1.0f, 0.0f));
    defaultShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    defaultShader->set_mat4("model", model);
    ufo_obj->draw(defaultShader);
}

void MainController::drawUFO2() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *lowpolyUFO = resources->model("Low_poly_ufo_OBJ");
    engine::resources::Shader *instancingShader = resources->shader("instancing");
    instancingShader->use();
    instancingShader->set_mat4("projection", graphics->projection_matrix());
    instancingShader->set_mat4("view", graphics->camera()
                                               ->view_matrix());
    glm::mat4 model = glm::mat4(0.1f);
    instancingShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    lowpolyUFO->prepareInstancing(ufoMatrices, ufoMatrices.size());
    lowpolyUFO->drawInstances(instancingShader, ufoMatrices.size());
}

void MainController::drawPlatform() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *ufo_obj = resources->model("Platform");
    engine::resources::Shader *ufo_shader = resources->shader("ufo");
    ufo_shader->use();
    ufo_shader->set_mat4("projection", graphics->projection_matrix());
    ufo_shader->set_mat4("view", graphics->camera()
                                         ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 12.0f, 0.0f));
    ufo_shader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    ufo_shader->set_mat4("model", model);
    ufo_obj->draw(ufo_shader);
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
    deferredRender();
    drawSkybox();
    engine::graphics::OpenGL::disable_culling();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto &spotLights = LightController::getSpotLights();
    t += platform->dt() * 10.0f;
    for (uint32_t i = 0; i < NR_SPOT_LIGHTS; i++) {
        float angle = t + 2 * i * M_PI / NR_SPOT_LIGHTS;
        spotLights[i].position = glm::vec3(50.0f * cos(angle), 35.0f, 200.0f + 50.0f * sin(angle));;
    }
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

std::string_view MainController::name() const {
    return "app::MainController";
}

}
