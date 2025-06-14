#include <LightController.hpp>
#include <GUIController.hpp>

namespace app {

static std::array<PointLight, NR_POINT_LIGHTS> pointLights;
static std::array<DirectionalLight, NR_DIR_LIGHTS> directionalLights;
static std::array<SpotLight, NR_SPOT_LIGHTS> spotLights;

void LightController::initialize() {
    for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
        updatePoint(
                PointLight(Light(lightColor, ambientStrength, diffuseStrength, specularStrength,
                                 1.0f, 0.09f, 0.232f, shininess),
                           glm::vec3(sin(2 * i * M_PI / NR_POINT_LIGHTS) * 80.0f, 4.0f,
                                     cos(2 * i * M_PI / NR_POINT_LIGHTS) * 80.0f)),
                i);
    }
    for (size_t i = 0; i < NR_DIR_LIGHTS; i++) {
        auto flipX = i % 2 ? 1.0f : -1.0f;
        if (i < 2) {
            updateDirectional(
                    DirectionalLight(Light(lightColor, ambientStrength, diffuseStrength, specularStrength,
                                           1.0f, 0.09f, 0.232f, shininess), glm::vec3(flipX * 1.0f, -1.0f, 1.0f)),
                    i);
        } else {
            updateDirectional(
                    DirectionalLight(Light(lightColor, ambientStrength, diffuseStrength, specularStrength,
                                           1.0f, 0.09f, 0.232f, shininess), glm::vec3(flipX * 1.0f, -1.0f, -1.0f)),
                    i);
        }

    }
    for (uint32_t i = 0; i < NR_SPOT_LIGHTS; i++) {
        updateSpot(SpotLight(Light(lightColor, ambientStrength, diffuseStrength, specularStrength,
                                          1.0f, 0.09f, 0.232f, shininess), glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 12.5f, 17.5f), i);
    }
}

void LightController::poll_events() {
    auto guiController = engine::core::Controller::get<GUIController>();
    if (guiController->is_enabled()) {
        set_enable(!is_enabled());
    }
}

void LightController::togglePoint(PointLight &light) {
    light.enabled = !light.enabled;
}

void LightController::updatePoint(PointLight newLight, size_t index) {
    RG_GUARANTEE(index >= 0 & index < NR_POINT_LIGHTS, "point light index out of range");
    pointLights[index] = newLight;
}

const std::array<PointLight, NR_POINT_LIGHTS> &LightController::getPointLights() {
    return pointLights;
}

void LightController::toggleDirectional(DirectionalLight &light) {
    light.enabled = !light.enabled;
}

void LightController::updateDirectional(DirectionalLight newLight, size_t index) {
    RG_GUARANTEE(index >= 0 & index < NR_DIR_LIGHTS, "directional light index out of range");
    directionalLights[index] = newLight;
}

const std::array<DirectionalLight, NR_DIR_LIGHTS> &LightController::getDirectionalLights() {
    return directionalLights;
}

void LightController::toggleSpot(SpotLight &light) {
    light.enabled = !light.enabled;
}

void LightController::updateSpot(SpotLight newLight, size_t index) {
    RG_GUARANTEE(index >= 0 & index < NR_SPOT_LIGHTS, "spotlight index out of range");
    spotLights[index] = newLight;
}

const std::array<SpotLight, NR_SPOT_LIGHTS> &LightController::getSpotLights() {
    return spotLights;
}

std::string_view LightController::name() const {
    return "app::LightController";
}

void LightController::setShaderPointLights(engine::resources::Shader *shader, const std::string &name,
                                           std::array<PointLight, NR_POINT_LIGHTS> &lights) {
    for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
        shader->set_vec3(name + "[" + std::to_string(i) + "].position", lights[i].position);
        shader->set_vec3(name + "[" + std::to_string(i) + "].color", lights[i].color);
        shader->set_vec3(name + "[" + std::to_string(i) + "].ambientStrength", lights[i].ambientStrength);
        shader->set_vec3(name + "[" + std::to_string(i) + "].diffuseStrength", lights[i].diffuseStrength);
        shader->set_vec3(name + "[" + std::to_string(i) + "].specularStrength", lights[i].specularStrength);
        shader->set_float(name + "[" + std::to_string(i) + "].constant", lights[i].constant);
        shader->set_float(name + "[" + std::to_string(i) + "].linear", lights[i].linear);
        shader->set_float(name + "[" + std::to_string(i) + "].quadratic", lights[i].quadratic);
        shader->set_float(name + "[" + std::to_string(i) + "].shininess", lights[i].shininess);
        shader->set_bool(name + "[" + std::to_string(i) + "].enabled", lights[i].enabled);
    }
}

void LightController::setShaderDirLights(engine::resources::Shader *shader, const std::string &name,
                                         std::array<DirectionalLight, NR_DIR_LIGHTS> &lights) {
    for (size_t i = 0; i < NR_DIR_LIGHTS; i++) {
        shader->set_vec3(name + "[" + std::to_string(i) + "].direction", lights[i].direction);
        shader->set_vec3(name + "[" + std::to_string(i) + "].color", lights[i].color);
        shader->set_vec3(name + "[" + std::to_string(i) + "].ambientStrength", lights[i].ambientStrength);
        shader->set_vec3(name + "[" + std::to_string(i) + "].diffuseStrength", lights[i].diffuseStrength);
        shader->set_vec3(name + "[" + std::to_string(i) + "].specularStrength", lights[i].specularStrength);
        shader->set_float(name + "[" + std::to_string(i) + "].constant", lights[i].constant);
        shader->set_float(name + "[" + std::to_string(i) + "].linear", lights[i].linear);
        shader->set_float(name + "[" + std::to_string(i) + "].quadratic", lights[i].quadratic);
        shader->set_float(name + "[" + std::to_string(i) + "].shininess", lights[i].shininess);
        shader->set_bool(name + "[" + std::to_string(i) + "].enabled", lights[i].enabled);
    }
}

void LightController::setShaderSpotLights(engine::resources::Shader *shader, const std::string &name,
                                          std::array<SpotLight, NR_SPOT_LIGHTS> &lights) {
    for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
        shader->set_vec3(name + "[" + std::to_string(i) + "].position", lights[i].position);
        shader->set_vec3(name + "[" + std::to_string(i) + "].direction", lights[i].direction);
        shader->set_vec3(name + "[" + std::to_string(i) + "].color", lights[i].color);
        shader->set_vec3(name + "[" + std::to_string(i) + "].ambientStrength", lights[i].ambientStrength);
        shader->set_vec3(name + "[" + std::to_string(i) + "].diffuseStrength", lights[i].diffuseStrength);
        shader->set_vec3(name + "[" + std::to_string(i) + "].specularStrength", lights[i].specularStrength);
        shader->set_float(name + "[" + std::to_string(i) + "].constant", lights[i].constant);
        shader->set_float(name + "[" + std::to_string(i) + "].linear", lights[i].linear);
        shader->set_float(name + "[" + std::to_string(i) + "].quadratic", lights[i].quadratic);
        shader->set_float(name + "[" + std::to_string(i) + "].shininess", lights[i].shininess);
        shader->set_float(name + "[" + std::to_string(i) + "].cutOff", lights[i].cutOff);
        shader->set_float(name + "[" + std::to_string(i) + "].outcutOff", lights[i].outerCutOff);
        shader->set_bool(name + "[" + std::to_string(i) + "].enabled", lights[i].enabled);
    }
}

}
