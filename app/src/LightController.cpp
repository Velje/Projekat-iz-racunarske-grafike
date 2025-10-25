#include <LightController.hpp>
#include <GUIController.hpp>

namespace app {

void LightController::initialize_lights() {
    // @formatter:off
    #include "pointlights.include"
    #include "spotlights.include"
    #include "dirlights.include"
    // @formatter:on
}

void LightController::initialize() {
    initialize_lights();
    auto resources = engine::core::Controller::get<ResourcesController>();
    Shader *lightShader = resources->shader("lightPass");
    lightShader->use();
    lightShader->set_int("gPosition", 0);
    lightShader->set_int("gNormal", 1);
    lightShader->set_int("gAlbedoSpec", 2);
    Shader::setup_ubo_lights(m_ubo_lights);
}

void LightController::poll_events() {

}

void LightController::update_point(PointLight &&new_light, size_t index) {
    RG_GUARANTEE(index < NR_POINT_LIGHTS, "point light index out of range");
    m_ubo_lights.point_lights[index] = new_light;
}

void LightController::toggle_point(PointLight &light) {
    light.base
         .enabled = !light.base
                          .enabled;
}

void LightController::update_directional(DirectionalLight &&new_light, size_t index) {
    RG_GUARANTEE(index < NR_DIR_LIGHTS, "dir light index out of range");
    m_ubo_lights.dir_lights[index] = new_light;
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
    m_ubo_lights.spot_lights[index] = new_light;
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
    lightShader->set_float("exposure", m_light_attributes.exposure);
    lightShader->set_float("gamma", m_light_attributes.gamma);
    Shader::update_lights(m_ubo_lights);
}

}
