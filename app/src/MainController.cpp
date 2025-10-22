#include <MainController.hpp>
#include <GUIController.hpp>
#include <MainPlatformEventObserver.hpp>
#include <LightController.hpp>
#include <EventController.hpp>

namespace app {

static std::unordered_map<engine::platform::KeyId, engine::graphics::Camera::Movement> KeyIdToCameraMovement;
static std::unordered_set<engine::platform::KeyId> keyControls;
static uint32_t gBuffer;
static std::array<uint32_t, 3> textureIDs;
static glm::mat4 UFOMatrix;
const size_t NR_UFO2_MODELS = 480, NR_UBO_MATRICES = 2;
static std::array<glm::mat4, NR_UFO2_MODELS> UFO2Matrices;
static float t = 0.0;
static bool toggleUFONormals = false, spotOn = true, pointOn = true, enteredUFO = false;
static std::vector<glm::mat4> uboMatrices(NR_UBO_MATRICES);

void initialize_keyid_maps();

const std::unordered_set<engine::platform::KeyId> &MainController::getKeyControls() {
    return keyControls;
}

void MainController::initialize() {
    auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
    auto resources = engine::core::Controller::get<ResourcesController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    KeyIdToCameraMovement.rehash(engine::graphics::Camera::Movement::MOVEMENT_COUNT);
    initialize_keyid_maps();
    engine::graphics::OpenGL::enable_depth_testing();
    textureIDs = engine::graphics::OpenGL::generateGbuffer(gBuffer, window->width(),
                                                           window->height());
    UFOMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 140.0f, 0.0f));
    float radius = 500.0f;
    size_t positionsPerCircle = 30;
    float yStep = 30.0f;
    for (size_t i = 0; i < NR_UFO2_MODELS; i++) {
        size_t circleIndex = i / positionsPerCircle;
        size_t positionInCircle = i % positionsPerCircle;

        float angle = (positionInCircle * 2.0f * M_PI) / positionsPerCircle;

        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        float y = -30.0f + (circleIndex * yStep);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(1.0f));
        UFO2Matrices[i] = model;
    }
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    camera->Position = glm::vec3(0.0, 157.0f, 0.0f);
}

void initialize_keyid_maps() {
    // @formatter:off
    #include "keyid_to_camera_movement.include"
    #include "keyid_controls.include"
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
    auto &uboLights = LightController::getUBOLightsReference();
    float actionStart, actionEnd, eventStart, eventEnd;
    if (platform->key(engine::platform::KeyId::KEY_P)
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->getGlfwTime();
        actionEnd = platform->getGlfwTime();
        eventStart = platform->getGlfwTime();
        pointOn = !pointOn;
        for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
            LightController::togglePoint(uboLights.pointLights[i]);
        }
        Shader::setupUBOLights(uboLights);
        eventEnd = platform->getGlfwTime();
        EventController::instaLog(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                         EventB::LIGHTS_TOGGLE_POINT));
    }
    if (platform->key(engine::platform::KeyId::KEY_I)
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->getGlfwTime();
        actionEnd = platform->getGlfwTime();
        eventStart = platform->getGlfwTime();
        for (size_t i = 0; i < NR_DIR_LIGHTS; i++) {
            LightController::toggleDirectional(uboLights.dirLights[i]);
        }
        Shader::setupUBOLights(uboLights);
        eventEnd = platform->getGlfwTime();
        EventController::instaLog(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                         EventB::LIGHTS_TOGGLE_DIR));
    }
    if (platform->key(engine::platform::KeyId::KEY_O)
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->getGlfwTime();
        actionEnd = platform->getGlfwTime();
        eventStart = platform->getGlfwTime();
        spotOn = !spotOn;
        for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
            LightController::toggleSpot(uboLights.spotLights[i]);
        }
        Shader::setupUBOLights(uboLights);
        eventEnd = platform->getGlfwTime();
        EventController::instaLog(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                         EventB::LIGHTS_TOGGLE_SPOTLIGHT));
    }
    if (platform->key((engine::platform::KeyId::KEY_N))
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->getGlfwTime();
        actionEnd = platform->getGlfwTime();
        eventStart = platform->getGlfwTime();
        toggleUFONormals = !toggleUFONormals;
        eventEnd = platform->getGlfwTime();
        EventController::instaLog(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                         EventB::MODEL_TOGGLE_NORMALS));
    }
    if (platform->key((engine::platform::KeyId::KEY_SPACE))
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->getGlfwTime();
        eventStart = platform->getGlfwTime();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        actionEnd = platform->getGlfwTime();
        camera->Position = glm::vec3(0.0f, 157.0f, 0.0f);
        UFOMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 140.0f, 0.0f));
        float radius = 500.0f;
        size_t positionsPerCircle = 30;
        float yStep = 30.0f;
        for (size_t i = 0; i < NR_UFO2_MODELS; i++) {
            size_t circleIndex = i / positionsPerCircle;
            size_t positionInCircle = i % positionsPerCircle;

            float angle = (positionInCircle * 2.0f * M_PI) / positionsPerCircle;

            float x = cos(angle) * radius;
            float z = sin(angle) * radius;

            float y = -30.0f + (circleIndex * yStep);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x, y, z));
            model = glm::scale(model, glm::vec3(1.0f));
            UFO2Matrices[i] = model;
        }
        eventEnd = platform->getGlfwTime();
        EventController::instaLog(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                         EventB::CAMERA_POSITION));
    }
    if (platform->key((engine::platform::KeyId::KEY_ENTER))
                .state() == engine::platform::Key::State::JustPressed) {
        enteredUFO = !enteredUFO;
    }
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
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
                if (enteredUFO) {
                    camera->MovementSpeed = 10000.0f;
                } else {
                    camera->MovementSpeed = 1000.0f;
                }
                eventEnd = platform->getGlfwTime();
                EventController::instaLog(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::CAMERA_SPEED_INCREASED));
            } else {
                if (enteredUFO) {
                    camera->MovementSpeed = 1000.0f;
                } else {
                    camera->MovementSpeed = 250.0f;
                }
                eventEnd = platform->getGlfwTime();
                EventController::instaLog(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::CAMERA_SPEED_STANDARD));
            }
            eventStart = platform->getGlfwTime();
            if (enteredUFO) {
                camera->Position =
                        glm::vec3(UFOMatrix[3]) - 1000.0f * camera->Front;
                updateModelPosition(UFOMatrix, pair.second);
                for (size_t i = 0; i < NR_UFO2_MODELS; i++) {
                    updateModelPosition(UFO2Matrices[i], pair.second);
                }
                camera->Position =
                        glm::vec3(UFOMatrix[3]) - 1000.0f * camera->Front;
                eventEnd = platform->getGlfwTime();
                EventController::instaLog(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::MODEL_POSITION));

            } else {
                camera->move_camera(pair.second, deltaTime);
                eventEnd = platform->getGlfwTime();
                EventController::instaLog(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::CAMERA_POSITION));
            }
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
    OpenGL::bindFrameBuffer(gBuffer);
    OpenGL::clear_buffers();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->perspective_params()
            .Near = 5.0f;
    graphics->perspective_params()
            .Far = 5000.0f;
    uboMatrices = {graphics->camera()
                           ->view_matrix(), graphics->projection_matrix()};
    Shader::setupUBOMatrices(uboMatrices);
    if (toggleUFONormals) {
        drawUFONormals();
    }
    drawTerrain();
    OpenGL::enable_backCulling();
    drawPlatform();
    drawUFO();
    drawUFO2();
    graphics->perspective_params()
            .Far = 1500000.0f;
    uboMatrices = {graphics->camera()
                           ->view_matrix(), graphics->projection_matrix()};
    Shader::setupUBOMatrices(uboMatrices);
    drawEarth();
    OpenGL::bindFrameBuffer(0);
}

void MainController::lightPass() {
    auto &uboLights = LightController::getUBOLightsReference();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    t += platform->dt();
    auto &pointLightPos = LightController::getLightPositions();
    if (spotOn) {
        for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
            float angle = t * 2.0f + 2 * i * M_PI / NR_SPOT_LIGHTS;
            uboLights.spotLights[i].position = glm::vec3(UFOMatrix[3]) + glm::vec3(48.0f * cos(angle),
                                                                                   60.0f,
                                                                                   -48.0f * sin(angle));
        }
    }
    if (pointOn) {
        for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
            float angle = t * 2.0f + 2 * i * M_PI / NR_SPOT_LIGHTS;
            uboLights.pointLights[i].position = glm::vec3(pointLightPos[i].first + 48.0f * cos(angle), 4.0f,
                                                          pointLightPos[i].second - 48.0f * sin(angle));
        }
    }
    LightController::updateLights();
    OpenGL::activateGbuffertextures(textureIDs);
    OpenGL::renderScreen();
}

void MainController::drawTerrain() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *brown_mud = resources->model("brown_mud");
    Shader *defaultShader = resources->shader("default");
    defaultShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.5f));
    defaultShader->set_mat4("model", model);
    defaultShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    brown_mud->draw(defaultShader);
}

void MainController::drawEarth() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *earth = resources->model("Earth");
    Shader *defaultShader = resources->shader("default");
    defaultShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -600000.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, 0.0000727f * t * 100.0f,
                        glm::vec3(0, 0.9171, 0.3987));
    model = glm::scale(model, glm::vec3(238000.0f));
    defaultShader->set_mat4("model", model);
    defaultShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    earth->draw(defaultShader);
}

void MainController::drawUFO() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *ufo_obj = resources->model("UFO");
    Shader *defaultShader = resources->shader("default");
    defaultShader->use();
    glm::mat4 model = UFOMatrix;
    float angle = 2.0f * t + 2.0f * M_PI / NR_SPOT_LIGHTS;
    model = glm::rotate(model, angle,
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(20.0f));
    defaultShader->set_mat4("model", model);
    defaultShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    ufo_obj->draw(defaultShader);
}

void MainController::drawUFO2() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *lowpolyUFO = resources->model("Low_poly_ufo_OBJ");
    Shader *instancingShader = resources->shader("instancing");
    instancingShader->use();
    float angle = 2.0f * t + 2.0f * M_PI / NR_SPOT_LIGHTS;
    std::vector<glm::mat4> models(NR_UFO2_MODELS);
    for (size_t i = 0; i < NR_UFO2_MODELS; i++) {
        models[i] = UFO2Matrices[i];
        models[i] = glm::rotate(models[i], angle,
                                glm::vec3(0.0f, 1.0f, 0.0f));
    }
    instancingShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(models[0]))));
    lowpolyUFO->drawInstances(instancingShader, models);
}

void MainController::drawUFONormals() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *ufo_obj = resources->model("UFO");
    Shader *normalsShader = resources->shader("normals");
    normalsShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = UFOMatrix;
    float angle = 2.0f * t + 2.0f * M_PI / NR_SPOT_LIGHTS;
    model = glm::rotate(model, angle,
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(20.0f));
    normalsShader->set_mat4("model", model);
    normalsShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    ufo_obj->draw(normalsShader);
}

void MainController::updateModelPosition(glm::mat4 &model, Camera::Movement &direction) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    glm::vec3 movementDir(0.0f);
    switch (direction) {
        case Camera::Movement::FORWARD: movementDir += camera->Front;
            break;
        case Camera::Movement::BACKWARD: movementDir -= camera->Front;
            break;
        case Camera::Movement::RIGHT: movementDir += camera->Right;
            break;
        case Camera::Movement::LEFT: movementDir -= camera->Right;
            break;
        case Camera::Movement::UP: movementDir += camera->WorldUp;
            break;
        case Camera::Movement::DOWN: movementDir -= camera->WorldUp;
            break;
        default: break;
    }
    model = glm::translate(model, platform->dt() * camera->MovementSpeed * glm::normalize(movementDir));
}

void MainController::drawPlatform() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *ufo_obj = resources->model("Platform");
    Shader *ufo_shader = resources->shader("default");
    ufo_shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 85.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.1f));
    ufo_shader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    ufo_shader->set_mat4("model", model);
    ufo_obj->draw(ufo_shader);
}

void MainController::drawSkybox() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    auto skybox = resources->skybox("skybox220");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::begin_draw() {
    OpenGL::clear_buffers();
}

void MainController::draw() {
    deferredRender();
    drawSkybox();
    OpenGL::disable_culling();
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

std::string_view MainController::name() const {
    return "app::MainController";
}

}
