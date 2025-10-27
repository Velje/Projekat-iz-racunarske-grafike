#include <MainController.hpp>

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
    m_main_controller = engine::core::Controller::get<MainController>();
    m_main_controller->m_light_controller = engine::core::Controller::get<LightController>();
    m_main_controller->m_event_controller = engine::core::Controller::get<EventController>();
    m_main_controller->m_platform_controller = engine::core::Controller::get<engine::platform::PlatformController>();
    m_main_controller->m_resources_controller = engine::core::Controller::get<ResourcesController>();
    m_main_controller->m_gui_controller = engine::core::Controller::get<GUIController>();
    m_main_controller->m_graphics_controller = engine::core::Controller::get<GraphicsController>();
    Shader *lightShader = m_main_controller->m_resources_controller
                                           ->shader("lightPass");
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
    Shader::update_lights(m_ubo_lights);
}

}
