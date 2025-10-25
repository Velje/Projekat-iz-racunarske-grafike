#include <GUIController.hpp>
#include <EventController.hpp>
#include <LightController.hpp>

namespace app {

static UBOLights &g_ubo_lights_reference = LightController::get_ubo_lights_reference();
static LightAttributes &g_light_attributes_reference = LightController::get_light_attributes_reference();
static int g_point_selector = 0, g_dir_selector = 0, g_spot_selector = 0;

void GUIController::initialize() {
    set_enable(false);
}

void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto eventController = engine::core::Controller::get<EventController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed
            ) {
        auto actionStart = platform->get_glfw_time();
        auto actionEnd = platform->get_glfw_time();
        auto eventStart = platform->get_glfw_time();
        eventController->set_enable(!eventController->is_enabled());
        Shader::setup_ubo_lights(g_ubo_lights_reference);
        set_enable(!is_enabled());
        auto eventEnd = platform->get_glfw_time();
        eventController->insta_log(
                Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                       EventB::GUI_TOGGLE));
    }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = graphics->camera();
    graphics->begin_gui();
    ImGui::Begin("GUI");
    ImGui::Text("Camera info");
    ImGui::Text("Camera position: (%f, %f, %f)", camera->Position
                                                       .x, camera->Position
                                                                 .y, camera->Position
                                                                           .z);
    ImGui::Text("Mouse position: %f %f", platform->mouse()
                                                 .x, platform->mouse()
                                                             .y);
    auto deltaTime = platform->dt();
    ImGui::Text("Frames per second: %f", 1 / deltaTime);
    ImGui::Text("Frame difference: %fms", deltaTime * 1000);
    ImGui::Text("Light control");
    ImGui::InputInt("Pointlight index", &g_point_selector);
    if (g_point_selector >= 0 && g_point_selector < NR_POINT_LIGHTS) {
        auto &pointLight = g_ubo_lights_reference.point_lights[g_point_selector];
        if (ImGui::RadioButton("Enabled1", pointLight.base
                                                     .enabled)) {
            pointLight.base
                      .enabled = !pointLight.base
                                            .enabled;
        }
        ImGui::ColorEdit3("Point light color", glm::value_ptr(pointLight.base
                                                                        .color));
        ImGui::DragFloat3("ambientStrength1", glm::value_ptr(pointLight.base
                                                                       .ambient_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("diffuseStrength1", glm::value_ptr(pointLight.base
                                                                       .diffuse_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("specularStrength1", glm::value_ptr(pointLight.base
                                                                        .specular_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat("shininess1", &pointLight.base
                                                  .shininess, 1.0f, 0.0f, 100.0f);
    }

    ImGui::InputInt("Dirlight index", &g_dir_selector);
    if (g_dir_selector >= 0 && g_dir_selector < NR_DIR_LIGHTS) {
        auto &dirLight = g_ubo_lights_reference.dir_lights[g_dir_selector];
        if (ImGui::RadioButton("Enabled2", dirLight.base
                                                   .enabled)) {
            dirLight.base
                    .enabled = !dirLight.base
                                        .enabled;
        }
        ImGui::ColorEdit3("Directional light color", glm::value_ptr(dirLight.base
                                                                            .color));
        ImGui::DragFloat3("ambientStrength2", glm::value_ptr(dirLight.base
                                                                     .ambient_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("diffuseStrength2", glm::value_ptr(dirLight.base
                                                                     .diffuse_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("specularStrength2", glm::value_ptr(dirLight.base
                                                                      .specular_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat("shininess2", &dirLight.base
                                                .shininess, 1.0f, 0.0f, 2048.0f);
    }

    ImGui::InputInt("Spotlight index", &g_spot_selector);
    if (g_spot_selector >= 0 && g_spot_selector < NR_SPOT_LIGHTS) {
        auto &spotLight = g_ubo_lights_reference.spot_lights[g_spot_selector];
        if (ImGui::RadioButton("Enabled3", spotLight.base
                                                    .enabled)) {
            spotLight.base
                     .enabled = !spotLight.base
                                          .enabled;
        }
        ImGui::ColorEdit3("Spotlight color", glm::value_ptr(spotLight.base
                                                                     .color));
        ImGui::DragFloat3("ambientStrength3", glm::value_ptr(spotLight.base
                                                                      .ambient_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("diffuseStrength3", glm::value_ptr(spotLight.base
                                                                      .diffuse_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("specularStrength3", glm::value_ptr(spotLight.base
                                                                       .specular_strength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat("shininess3", &spotLight.base
                                                 .shininess, 1.0f, 0.0f, 2048.0f);
    }
    ImGui::DragFloat("exposure", &g_light_attributes_reference.exposure, 0.25f, 0.0f, 100.0f);
    ImGui::DragFloat("gamma", &g_light_attributes_reference.gamma, 0.25f, 0.0f, 100.0f);
    ImGui::End();
    graphics->end_gui();
}

std::string_view GUIController::name() const {
    return "app::GUIController";
}

}
