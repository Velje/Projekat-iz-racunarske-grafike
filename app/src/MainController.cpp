#include <MainController.hpp>

namespace app {

static std::unordered_map<engine::platform::KeyId, engine::graphics::Camera::Movement> g_key_id_to_camera_movement;
static std::unordered_set<engine::platform::KeyId> g_key_controls;
static std::vector<glm::mat4> g_ufo_2_matrices(NR_UFO2_MODELS);
static std::vector<glm::mat4> g_ubo_matrices(NR_UBO_MATRICES);
static glm::mat4 g_ufo_matrix;
static glm::mat4 g_translation_matrix(1.0f);

static bool g_toggle_ufo_normals = false, g_spot_on = true, g_point_on = true, g_entered_ufo = false;
static float t = 0.0;

void initialize_keyid_maps();

const std::unordered_set<engine::platform::KeyId> &MainController::get_key_controls() {
    return g_key_controls;
}

void MainController::initialize() {
    auto window = m_platform_controller->window();
    m_platform_controller->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    g_key_id_to_camera_movement.rehash(Camera::Movement::MOVEMENT_COUNT);
    initialize_keyid_maps();
    OpenGL::enable_depth_testing();
    OpenGL::generate_gbuffer(m_graphics_controller->m_g_buffer,
                             m_graphics_controller->m_g_texture_ids,
                             window->width(),
                             window->height());
    OpenGL::generate_screen_vao(m_graphics_controller->m_screen_vao);
    g_ufo_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 140.0f, 0.0f));
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
        g_ufo_2_matrices[i] = model;
    }
    Model *lowpolyUFO = m_resources_controller->model("Low_poly_ufo_OBJ");
    lowpolyUFO->prepare_instancing_data(g_ufo_2_matrices);
    auto camera = m_graphics_controller->camera();
    camera->Position = glm::vec3(0.0, 157.0f, 0.0f);
}

void initialize_keyid_maps() {
    // @formatter:off
    #include "keyid_to_camera_movement.include"
    #include "keyid_controls.include"
    // @formatter:on
}

bool MainController::loop() {
    if (m_platform_controller->key(engine::platform::KeyId::KEY_ESCAPE)
                             .is_down()) {
        spdlog::info("ESC pressed. Exiting...");
        return false;
    }
    return true;
}

void MainController::poll_events() {
    auto &uboLights = m_light_controller->m_ubo_lights;
    float actionStart, actionEnd, eventStart, eventEnd;
    if (m_platform_controller->key(engine::platform::KeyId::KEY_P)
                             .state() == engine::platform::Key::State::JustPressed) {
        actionStart = m_platform_controller->get_glfw_time();
        actionEnd = m_platform_controller->get_glfw_time();
        eventStart = m_platform_controller->get_glfw_time();
        g_point_on = !g_point_on;
        for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
            m_light_controller->toggle_point(uboLights.point_lights[i]);
        }
        Shader::setup_ubo_lights(uboLights);
        eventEnd = m_platform_controller->get_glfw_time();
        m_event_controller->insta_log(
                Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                       EventB::LIGHTS_TOGGLE_POINT));
    }
    if (m_platform_controller->key(engine::platform::KeyId::KEY_I)
                             .state() == engine::platform::Key::State::JustPressed) {
        actionStart = m_platform_controller->get_glfw_time();
        actionEnd = m_platform_controller->get_glfw_time();
        eventStart = m_platform_controller->get_glfw_time();
        for (size_t i = 0; i < NR_DIR_LIGHTS; i++) {
            m_light_controller->toggle_directional(uboLights.dir_lights[i]);
        }
        Shader::setup_ubo_lights(uboLights);
        eventEnd = m_platform_controller->get_glfw_time();
        m_event_controller->insta_log(
                Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                       EventB::LIGHTS_TOGGLE_DIR));
    }
    if (m_platform_controller->key(engine::platform::KeyId::KEY_O)
                             .state() == engine::platform::Key::State::JustPressed) {
        actionStart = m_platform_controller->get_glfw_time();
        actionEnd = m_platform_controller->get_glfw_time();
        eventStart = m_platform_controller->get_glfw_time();
        g_spot_on = !g_spot_on;
        for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
            m_light_controller->toggle_spot(uboLights.spot_lights[i]);
        }
        Shader::setup_ubo_lights(uboLights);
        eventEnd = m_platform_controller->get_glfw_time();
        m_event_controller->insta_log(
                Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                       EventB::LIGHTS_TOGGLE_SPOTLIGHT));
    }
    if (m_platform_controller->key((engine::platform::KeyId::KEY_N))
                             .state() == engine::platform::Key::State::JustPressed) {
        actionStart = m_platform_controller->get_glfw_time();
        actionEnd = m_platform_controller->get_glfw_time();
        eventStart = m_platform_controller->get_glfw_time();
        g_toggle_ufo_normals = !g_toggle_ufo_normals;
        eventEnd = m_platform_controller->get_glfw_time();
        m_event_controller->insta_log(
                Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                       EventB::MODEL_TOGGLE_NORMALS));
    }
    if (m_platform_controller->key((engine::platform::KeyId::KEY_SPACE))
                             .state() == engine::platform::Key::State::JustPressed) {
        actionStart = m_platform_controller->get_glfw_time();
        eventStart = m_platform_controller->get_glfw_time();
        auto camera = m_graphics_controller->camera();
        actionEnd = m_platform_controller->get_glfw_time();
        camera->Position = glm::vec3(0.0f, 157.0f, 0.0f);
        g_ufo_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 140.0f, 0.0f));
        g_translation_matrix = glm::mat4(1.0f);
        eventEnd = m_platform_controller->get_glfw_time();
        m_event_controller->insta_log(
                Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                       EventB::CAMERA_POSITION));
    }
    if (m_platform_controller->key((engine::platform::KeyId::KEY_ENTER))
                             .state() == engine::platform::Key::State::JustPressed) {
        g_entered_ufo = !g_entered_ufo;
    }
}

void MainController::update_camera() {
    auto camera = m_graphics_controller->camera();
    auto deltaTime = m_platform_controller->dt();
    for (auto &pair: g_key_id_to_camera_movement) {
        if (m_platform_controller->key(pair.first)
                                 .is_down()) {
            float actionStart = m_platform_controller->get_glfw_time();
            float actionEnd = m_platform_controller->get_glfw_time();
            float eventStart = m_platform_controller->get_glfw_time();
            float eventEnd;
            if (m_platform_controller->key(engine::platform::KeyId::KEY_LEFT_SHIFT)
                                     .is_down()) {
                if (g_entered_ufo) {
                    camera->MovementSpeed = 10000.0f;
                } else {
                    camera->MovementSpeed = 1000.0f;
                }
                eventEnd = m_platform_controller->get_glfw_time();
                m_event_controller->insta_log(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::CAMERA_SPEED_INCREASED));
            } else {
                if (g_entered_ufo) {
                    camera->MovementSpeed = 1000.0f;
                } else {
                    camera->MovementSpeed = 250.0f;
                }
                eventEnd = m_platform_controller->get_glfw_time();
                m_event_controller->insta_log(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::CAMERA_SPEED_STANDARD));
            }
            eventStart = m_platform_controller->get_glfw_time();
            if (g_entered_ufo) {
                camera->Position =
                        glm::vec3(g_ufo_matrix[3]) - 1000.0f * camera->Front;
                update_model_position(g_ufo_matrix, pair.second);
                update_model_position(g_translation_matrix, pair.second);
                camera->Position =
                        glm::vec3(g_ufo_matrix[3]) - 1000.0f * camera->Front;
                eventEnd = m_platform_controller->get_glfw_time();
                m_event_controller->insta_log(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::MODEL_POSITION));

            } else {
                camera->move_camera(pair.second, deltaTime);
                eventEnd = m_platform_controller->get_glfw_time();
                m_event_controller->insta_log(
                        Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                               EventB::CAMERA_POSITION));
            }
        }
    }
}

void MainController::update() {
    if (m_gui_controller->is_enabled()) {
        m_platform_controller->set_enable_cursor(true);
        return;
    } else {
        m_platform_controller->set_enable_cursor(false);
        update_camera();
    }
}

void MainController::deferred_render() {
    geometry_pass();
    light_pass();
    auto window = engine::core::Controller::get<engine::platform::PlatformController>()->window();
    OpenGL::write_to_default_framebuffer(m_graphics_controller->m_g_buffer, window->width(), window->height());
}

void MainController::geometry_pass() {
    OpenGL::bind_frame_buffer(m_graphics_controller->m_g_buffer);
    OpenGL::clear_buffers();
    m_graphics_controller->perspective_params()
                         .Near = 5.0f;
    m_graphics_controller->perspective_params()
                         .Far = 5000.0f;
    g_ubo_matrices = {m_graphics_controller->camera()
                                           ->view_matrix(), m_graphics_controller->projection_matrix()};
    Shader::setup_ubo_matrices(g_ubo_matrices);
    if (g_toggle_ufo_normals) {
        draw_ufo_normals();
    }
    draw_terrain();
    OpenGL::enable_back_culling();
    draw_platform();
    draw_ufo();
    draw_ufo_2();
    m_graphics_controller->perspective_params()
                         .Far = 1500000.0f;
    g_ubo_matrices = {m_graphics_controller->camera()
                                           ->view_matrix(), m_graphics_controller->projection_matrix()};
    Shader::setup_ubo_matrices(g_ubo_matrices);
    draw_earth();
    OpenGL::bind_frame_buffer(0);
}

void MainController::light_pass() {
    auto &uboLights = m_light_controller->m_ubo_lights;
    t += m_platform_controller->dt();
    auto &pointLightPos = m_light_controller->m_initial_pointlight_positions;
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
            uboLights.point_lights[i].position = pointLightPos[i] + glm::vec3(48.0f * cos(angle), 4.0f,
                                                                              48.0f * sin(angle));
        }
    }
    Shader *lightShader = m_resources_controller
            ->shader("lightPass");
    lightShader->use();
    lightShader->set_vec3("viewPos", m_graphics_controller
            ->camera()
            ->Position);
    lightShader->set_float("exposure", m_light_controller->m_light_attributes
                                                         .exposure);
    lightShader->set_float("gamma", m_light_controller->m_light_attributes
                                                      .gamma);
    if (g_spot_on || g_point_on) {
        m_light_controller->update_lights();
    }
    OpenGL::activate_gbuffertextures(m_graphics_controller->m_g_texture_ids);
    OpenGL::render_screen(m_graphics_controller->m_screen_vao);
}

void MainController::draw_terrain() {
    Model *brown_mud = m_resources_controller->model("brown_mud");
    Shader *defaultShader = m_resources_controller->shader("default");
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
    Model *earth = m_resources_controller->model("Earth");
    Shader *defaultShader = m_resources_controller->shader("default");
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
    Model *ufo_obj = m_resources_controller->model("UFO");
    Shader *defaultShader = m_resources_controller->shader("default");
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
    Model *lowpolyUFO = m_resources_controller->model("Low_poly_ufo_OBJ");
    Shader *instancingShader = m_resources_controller->shader("instancing");
    instancingShader->use();
    float angle = 2.0f * t + 2.0f * M_PI / NR_SPOT_LIGHTS;
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    instancingShader->set_mat4("transformedModel", g_translation_matrix * rotationMatrix);
    instancingShader->set_mat3("normalModelMatrix",
                               glm::mat3(glm::transpose(glm::inverse(rotationMatrix))));
    lowpolyUFO->draw_instances(instancingShader, NR_UFO2_MODELS);
}

void MainController::draw_ufo_normals() {
    Model *ufo_obj = m_resources_controller->model("UFO");
    Shader *normalsShader = m_resources_controller->shader("normals");
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
    auto camera = m_graphics_controller->camera();
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
    model = glm::translate(model, m_platform_controller->dt() * camera->MovementSpeed * glm::normalize(movementDir));
}

void MainController::draw_platform() {
    Model *ufo_obj = m_resources_controller->model("Platform");
    Shader *ufo_shader = m_resources_controller->shader("default");
    ufo_shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 85.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.1f));
    ufo_shader->set_mat3("normalModelMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
    ufo_shader->set_mat4("model", model);
    ufo_obj->draw(ufo_shader);
}

void MainController::draw_skybox() {
    auto skybox = m_resources_controller->skybox("skybox220");
    auto shader = m_resources_controller->shader("skybox");
    m_graphics_controller->draw_skybox(shader, skybox);
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
