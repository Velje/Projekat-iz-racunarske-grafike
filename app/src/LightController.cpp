#include <LightController.hpp>
#include <GUIController.hpp>

namespace app {

static UBOLights g_ubo_lights;
static LightAttributes g_light_attributes;
static std::vector<std::pair<float, float>> g_pointlight_positions;

LightAttributes &LightController::get_light_attributes_reference() {
    return g_light_attributes;
}

UBOLights &LightController::get_ubo_lights_reference() {
    return g_ubo_lights;
}

std::vector<std::pair<float, float>> &LightController::get_pointlight_positions() {
    return g_pointlight_positions;
}

void LightController::initialize() {
    const uint32_t gridSize = 8;
    const float spacing = 200.0f / gridSize;

    for (uint32_t row = 0; row < gridSize; ++row) {
        for (uint32_t col = 0; col < gridSize; ++col) {
            float x = -100.0f + col * spacing + spacing / 2.0f;
            float z = -100.0f + row * spacing + spacing / 2.0f;
            g_pointlight_positions.emplace_back(x, z);
        }
    }
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
        size_t ostatak = i % 4;
        lightColor = glm::vec3(ostatak == 0 || ostatak == 3, ostatak == 1, ostatak == 2 || ostatak == 3);
        update_point(PointLight(Light(lightColor, true),
                                glm::vec3(g_pointlight_positions[i].first, 4.0f, g_pointlight_positions[i].second)), i);
    }
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < NR_DIR_LIGHTS; i++) {
        auto flipX = i % 2 ? 1.0f : -1.0f;
        if (i < 2) {
            update_directional(DirectionalLight(Light(lightColor, true), glm::vec3(flipX * 1.0f, -1.0f, 1.0f)), i);
        } else {
            update_directional(DirectionalLight(Light(lightColor, true), glm::vec3(flipX * 1.0f, -1.0f, -1.0f)), i);
        }
    }
    for (uint32_t i = 0; i < NR_SPOT_LIGHTS; i++) {
        size_t ostatak = i % 4;
        lightColor = glm::vec3(ostatak == 0 || ostatak == 3, ostatak == 1, ostatak == 2 || ostatak == 3);
        double angle = 2 * i * M_PI / NR_SPOT_LIGHTS;
        update_spot(
                SpotLight(Light(lightColor, true), glm::vec3(48.0f * cos(angle), 200.0f, -48.0f * sin(angle)),
                          glm::vec3(0.0f, -1.0f, 0.0f), cos(glm::radians(1.0f)), cos(glm::radians(2.0f))),
                i);
    }
    auto resources = engine::core::Controller::get<ResourcesController>();
    Shader *lightShader = resources->shader("lightPass");
    lightShader->use();
    lightShader->set_int("gPosition", 0);
    lightShader->set_int("gNormal", 1);
    lightShader->set_int("gAlbedoSpec", 2);
    Shader::setup_ubo_lights(g_ubo_lights);
}

void LightController::poll_events() {

}

void LightController::update_point(PointLight &&new_light, size_t index) {
    RG_GUARANTEE(index < NR_POINT_LIGHTS, "point light index out of range");
    g_ubo_lights.point_lights[index] = new_light;
}

void LightController::toggle_point(PointLight &light) {
    light.base
         .enabled = !light.base
                          .enabled;
}

void LightController::update_directional(DirectionalLight &&new_light, size_t index) {
    RG_GUARANTEE(index < NR_DIR_LIGHTS, "dir light index out of range");
    g_ubo_lights.dir_lights[index] = new_light;
}

void LightController::toggle_directional(DirectionalLight &light) {
    light.base
         .enabled = !light.base
                          .enabled;
}

void LightController::toggle_spot(SpotLight &light) {
    light.base
         .enabled = !light.base
                          .enabled;
}

void LightController::update_spot(SpotLight &&new_light, size_t index) {
    RG_GUARANTEE(index < NR_SPOT_LIGHTS, "spot light index out of range");
    g_ubo_lights.spot_lights[index] = new_light;
}

std::string_view LightController::name() const {
    return "app::LightController";
}

void LightController::update_lights() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    Shader *lightShader = resources->shader("lightPass");
    lightShader->use();
    lightShader->set_vec3("viewPos", graphics->camera()
                                             ->Position);
    lightShader->set_float("exposure", g_light_attributes.exposure);
    lightShader->set_float("gamma", g_light_attributes.gamma);
    Shader::update_lights(g_ubo_lights);
}

}
