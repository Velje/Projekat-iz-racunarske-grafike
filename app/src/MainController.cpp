#include <MainController.hpp>
#include <GUIController.hpp>
#include <MainPlatformEventObserver.hpp>
#include <LightController.hpp>
#include <EventController.hpp>

namespace app {

static std::unordered_map<engine::platform::KeyId, engine::graphics::Camera::Movement> g_key_id_to_camera_movement;
static std::unordered_set<engine::platform::KeyId> g_key_controls;
static uint32_t g_g_buffer;
static std::array<uint32_t, 3> g_texture_ids;
static glm::mat4 g_ufo_matrix;
const size_t g_nr_ufo_2_models = 480, g_nr_ubo_matrices = 2;
static std::array<glm::mat4, g_nr_ufo_2_models> g_ufo_2_matrices;
static float t = 0.0;
static bool g_toggle_ufo_normals = false, g_spot_on = true, g_point_on = true, g_entered_ufo = false;
static std::vector<glm::mat4> g_ubo_matrices(g_nr_ubo_matrices);

void initialize_keyid_maps();

const std::unordered_set<engine::platform::KeyId> &MainController::get_key_controls() {
    return g_key_controls;
}

void MainController::initialize() {
    auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
    auto resources = engine::core::Controller::get<ResourcesController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    g_key_id_to_camera_movement.rehash(engine::graphics::Camera::Movement::MOVEMENT_COUNT);
    initialize_keyid_maps();
    engine::graphics::OpenGL::enable_depth_testing();
    g_texture_ids = engine::graphics::OpenGL::generate_gbuffer(g_g_buffer, window->width(),
                                                               window->height());
    g_ufo_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 140.0f, 0.0f));
    float radius = 500.0f;
    size_t positionsPerCircle = 30;
    float yStep = 30.0f;
    for (size_t i = 0; i < g_nr_ufo_2_models; i++) {
        size_t circleIndex = i / positionsPerCircle;
        size_t positionInCircle = i % positionsPerCircle;

        float angle = (positionInCircle * 2.0f * M_PI) / positionsPerCircle;

        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        float y = -30.0f + (circleIndex * yStep);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(1.0f));
        g_ufo_2_matrices[i] = model;
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
    auto &uboLights = LightController::get_ubo_lights_reference();
    float actionStart, actionEnd, eventStart, eventEnd;
    if (platform->key(engine::platform::KeyId::KEY_P)
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->get_glfw_time();
        actionEnd = platform->get_glfw_time();
        eventStart = platform->get_glfw_time();
        g_point_on = !g_point_on;
        for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
            LightController::toggle_point(uboLights.point_lights[i]);
        }
        Shader::setup_ubo_lights(uboLights);
        eventEnd = platform->get_glfw_time();
        EventController::insta_log(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                          EventB::LIGHTS_TOGGLE_POINT));
    }
    if (platform->key(engine::platform::KeyId::KEY_I)
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->get_glfw_time();
        actionEnd = platform->get_glfw_time();
        eventStart = platform->get_glfw_time();
        for (size_t i = 0; i < NR_DIR_LIGHTS; i++) {
            LightController::toggle_directional(uboLights.dir_lights[i]);
        }
        Shader::setup_ubo_lights(uboLights);
        eventEnd = platform->get_glfw_time();
        EventController::insta_log(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                          EventB::LIGHTS_TOGGLE_DIR));
    }
    if (platform->key(engine::platform::KeyId::KEY_O)
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->get_glfw_time();
        actionEnd = platform->get_glfw_time();
        eventStart = platform->get_glfw_time();
        g_spot_on = !g_spot_on;
        for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
            LightController::toggle_spot(uboLights.spot_lights[i]);
        }
        Shader::setup_ubo_lights(uboLights);
        eventEnd = platform->get_glfw_time();
        EventController::insta_log(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                          EventB::LIGHTS_TOGGLE_SPOTLIGHT));
    }
    if (platform->key((engine::platform::KeyId::KEY_N))
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->get_glfw_time();
        actionEnd = platform->get_glfw_time();
        eventStart = platform->get_glfw_time();
        g_toggle_ufo_normals = !g_toggle_ufo_normals;
        eventEnd = platform->get_glfw_time();
        EventController::insta_log(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                          EventB::MODEL_TOGGLE_NORMALS));
    }
    if (platform->key((engine::platform::KeyId::KEY_SPACE))
                .state() == engine::platform::Key::State::JustPressed) {
        actionStart = platform->get_glfw_time();
        eventStart = platform->get_glfw_time();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        actionEnd = platform->get_glfw_time();
        camera->Position = glm::vec3(0.0f, 157.0f, 0.0f);
        g_ufo_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 140.0f, 0.0f));
        float radius = 500.0f;
        size_t positionsPerCircle = 30;
        float yStep = 30.0f;
        for (size_t i = 0; i < g_nr_ufo_2_models; i++) {
            size_t circleIndex = i / positionsPerCircle;
            size_t positionInCircle = i % positionsPerCircle;

            float angle = (positionInCircle * 2.0f * M_PI) / positionsPerCircle;

            float x = cos(angle) * radius;
            float z = sin(angle) * radius;

            float y = -30.0f + (circleIndex * yStep);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x, y, z));
            model = glm::scale(model, glm::vec3(1.0f));
            g_ufo_2_matrices[i] = model;
        }
        eventEnd = platform->get_glfw_time();
        EventController::insta_log(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                                          EventB::CAMERA_POSITION));
    }
    if (platform->key((engine::platform::KeyId::KEY_ENTER))
                .state() == engine::platform::Key::State::JustPressed) {
        g_entered_ufo = !g_entered_ufo;
    }
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    auto deltaTime = platform->dt();
    for (auto &pair: g_key_id_to_camera_movement) {
        if (platform->key(pair.first)
                    .is_down()) {
            float actionStart = platform->get_glfw_time();
            float actionEnd = platform->get_glfw_time();
            float eventStart = platform->get_glfw_time();
            float eventEnd;
            if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT)
                        .is_down()) {
                if (g_entered_ufo) {
                    camera->MovementSpeed = 10000.0f;
                } else {
                    camera->MovementSpeed = 1000.0f;
                }
                eventEnd = platform->get_glfw_time();
                EventController::insta_log(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::CAMERA_SPEED_INCREASED));
            } else {
                if (g_entered_ufo) {
                    camera->MovementSpeed = 1000.0f;
                } else {
                    camera->MovementSpeed = 250.0f;
                }
                eventEnd = platform->get_glfw_time();
                EventController::insta_log(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::CAMERA_SPEED_STANDARD));
            }
            eventStart = platform->get_glfw_time();
            if (g_entered_ufo) {
                camera->Position =
                        glm::vec3(g_ufo_matrix[3]) - 1000.0f * camera->Front;
                update_model_position(g_ufo_matrix, pair.second);
                for (size_t i = 0; i < g_nr_ufo_2_models; i++) {
                    update_model_position(g_ufo_2_matrices[i], pair.second);
                }
                camera->Position =
                        glm::vec3(g_ufo_matrix[3]) - 1000.0f * camera->Front;
                eventEnd = platform->get_glfw_time();
                EventController::insta_log(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::MODEL_POSITION));

            } else {
                camera->move_camera(pair.second, deltaTime);
                eventEnd = platform->get_glfw_time();
                EventController::insta_log(
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

void MainController::deferred_render() {
    geometry_pass();
    light_pass();
    auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
    engine::graphics::OpenGL::write_to_default_framebuffer(g_g_buffer, window->width(), window->height());
}

void MainController::geometry_pass() {
    OpenGL::bind_frame_buffer(g_g_buffer);
    OpenGL::clear_buffers();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->perspective_params()
            .Near = 5.0f;
    graphics->perspective_params()
            .Far = 5000.0f;
    g_ubo_matrices = {graphics->camera()
                              ->view_matrix(), graphics->projection_matrix()};
    Shader::setup_ubo_matrices(g_ubo_matrices);
    if (g_toggle_ufo_normals) {
        draw_ufo_normals();
    }
    draw_terrain();
    OpenGL::enable_back_culling();
    draw_platform();
    draw_ufo();
    draw_ufo_2();
    graphics->perspective_params()
            .Far = 1500000.0f;
    g_ubo_matrices = {graphics->camera()
                              ->view_matrix(), graphics->projection_matrix()};
    Shader::setup_ubo_matrices(g_ubo_matrices);
    draw_earth();
    OpenGL::bind_frame_buffer(0);
}

void MainController::light_pass() {
    auto &uboLights = LightController::get_ubo_lights_reference();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    t += platform->dt();
    auto &pointLightPos = LightController::get_pointlight_positions();
    if (g_spot_on) {
        for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
            float angle = t * 2.0f + 2 * i * M_PI / NR_SPOT_LIGHTS;
            uboLights.spot_lights[i].position = glm::vec3(g_ufo_matrix[3]) + glm::vec3(48.0f * cos(angle),
                                                                                       60.0f,
                                                                                       -48.0f * sin(angle));
        }
    }
    if (g_point_on) {
        for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
            float angle = t * 2.0f + 2 * i * M_PI / NR_SPOT_LIGHTS;
            uboLights.point_lights[i].position = glm::vec3(pointLightPos[i].first + 48.0f * cos(angle), 4.0f,
                                                           pointLightPos[i].second - 48.0f * sin(angle));
        }
    }
    LightController::update_lights();
    OpenGL::activate_gbuffertextures(g_texture_ids);
    OpenGL::render_screen();
}

void MainController::draw_terrain() {
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

void MainController::draw_earth() {
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

void MainController::draw_ufo() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *ufo_obj = resources->model("UFO");
    Shader *defaultShader = resources->shader("default");
    defaultShader->use();
    glm::mat4 model = g_ufo_matrix;
    float angle = 2.0f * t + 2.0f * M_PI / NR_SPOT_LIGHTS;
    model = glm::rotate(model, angle,
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(20.0f));
    defaultShader->set_mat4("model", model);
    defaultShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    ufo_obj->draw(defaultShader);
}

void MainController::draw_ufo_2() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *lowpolyUFO = resources->model("Low_poly_ufo_OBJ");
    Shader *instancingShader = resources->shader("instancing");
    instancingShader->use();
    float angle = 2.0f * t + 2.0f * M_PI / NR_SPOT_LIGHTS;
    std::vector<glm::mat4> models(g_nr_ufo_2_models);
    for (size_t i = 0; i < g_nr_ufo_2_models; i++) {
        models[i] = g_ufo_2_matrices[i];
        models[i] = glm::rotate(models[i], angle,
                                glm::vec3(0.0f, 1.0f, 0.0f));
    }
    instancingShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(models[0]))));
    lowpolyUFO->draw_instances(instancingShader, models);
}

void MainController::draw_ufo_normals() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    Model *ufo_obj = resources->model("UFO");
    Shader *normalsShader = resources->shader("normals");
    normalsShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = g_ufo_matrix;
    float angle = 2.0f * t + 2.0f * M_PI / NR_SPOT_LIGHTS;
    model = glm::rotate(model, angle,
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(20.0f));
    normalsShader->set_mat4("model", model);
    normalsShader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    ufo_obj->draw(normalsShader);
}

void MainController::update_model_position(glm::mat4 &model, Camera::Movement &direction) {
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

void MainController::draw_platform() {
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

void MainController::draw_skybox() {
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
    deferred_render();
    draw_skybox();
    OpenGL::disable_culling();
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

std::string_view MainController::name() const {
    return "app::MainController";
}

}
