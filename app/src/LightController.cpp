#include <LightController.hpp>
#include <GUIController.hpp>

namespace app {

static std::array<PointLight, NR_POINT_LIGHTS> pointLights;
static std::array<DirectionalLight, NR_DIR_LIGHTS> directionalLights;
static std::array<SpotLight, NR_SPOT_LIGHTS> spotLights;

void LightController::initialize() {
    std::vector<std::pair<float, float>> lights;
    const uint32_t gridSize = 8;
    const float spacing = 200.0f / gridSize;

    for (uint32_t row = 0; row < gridSize; ++row) {
        for (uint32_t col = 0; col < gridSize; ++col) {
            float x = -100.0f + col * spacing + spacing / 2.0f;
            float z = -100.0f + row * spacing + spacing / 2.0f;
            lights.emplace_back(x, z);
        }
    }
    glm::mat4 model = glm::mat4(1.0f);
    for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
        if (i % 4 == 0) {
            lightColor = glm::vec3(1.0f, 0.0f, 0.0f);
        } else if (i % 4 == 1) {
            lightColor = glm::vec3(0.0f, 1.0f, 0.0f);
        } else if (i % 4 == 2) {
            lightColor = glm::vec3(0.0f, 0.0f, 1.0f);
        } else {
            lightColor = glm::vec3(1.0f, 0.0f, 1.0f);
        }
        updatePoint(
                PointLight(
                        Light(lightColor, 3.0f * ambientStrength,
                              diffuseStrength,
                              specularStrength,
                              1.0f, 0.0f, 0.0, shininess),
                        glm::vec3(lights[i].first, 4.0f,
                                  200.0f + lights[i].second)),
                i);
    }
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < NR_DIR_LIGHTS; i++) {
        auto flipX = i % 2 ? 1.0f : -1.0f;
        if (i < 2) {
            updateDirectional(
                    DirectionalLight(Light(lightColor, ambientStrength, diffuseStrength, specularStrength,
                                           1.0f, 0.0f, 0.0f, shininess, false),
                                     glm::vec3(flipX * 1.0f, -1.0f, 1.0f)),
                    i);
        } else {
            updateDirectional(
                    DirectionalLight(Light(lightColor, ambientStrength, diffuseStrength, specularStrength,
                                           1.0f, 0.0f, 0.0f, shininess, false),
                                     glm::vec3(flipX * 1.0f, -1.0f, -1.0f)),
                    i);
        }
    }
    for (uint32_t i = 0; i < NR_SPOT_LIGHTS; i++) {
        if (i % 4 == 0) {
            lightColor = glm::vec3(1.0f, 0.0f, 0.0f);
        } else if (i % 4 == 1) {
            lightColor = glm::vec3(0.0f, 1.0f, 0.0f);
        } else if (i % 4 == 2) {
            lightColor = glm::vec3(0.0f, 0.0f, 1.0f);
        } else {
            lightColor = glm::vec3(1.0f, 0.0f, 1.0f);
        }
        double angle = 2 * i * M_PI / NR_SPOT_LIGHTS;
        updateSpot(SpotLight(Light(lightColor, 3.0f * ambientStrength, diffuseStrength, specularStrength,
                                   1.0f, 0.0f, 0.0f, shininess),
                             glm::vec3(50.0f * cos(angle), 35.0f,
                                       200.0f - 50.0f * sin(angle)),
                             glm::vec3(0.0f, -1.0f, 0.0f), cos(glm::radians(5.0f)), cos(glm::radians(15.0f))), i);
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

std::array<PointLight, NR_POINT_LIGHTS> &LightController::getPointLights() {
    return pointLights;
}

void LightController::toggleDirectional(DirectionalLight &light) {
    light.enabled = !light.enabled;
}

void LightController::updateDirectional(DirectionalLight newLight, size_t index) {
    RG_GUARANTEE(index >= 0 & index < NR_DIR_LIGHTS, "directional light index out of range");
    directionalLights[index] = newLight;
}

std::array<DirectionalLight, NR_DIR_LIGHTS> &LightController::getDirectionalLights() {
    return directionalLights;
}

void LightController::toggleSpot(SpotLight &light) {
    light.enabled = !light.enabled;
}

void LightController::updateSpot(SpotLight newLight, size_t index) {
    RG_GUARANTEE(index >= 0 & index < NR_SPOT_LIGHTS, "spotlight index out of range");
    spotLights[index] = newLight;
}

std::array<SpotLight, NR_SPOT_LIGHTS> &LightController::getSpotLights() {
    return spotLights;
}

std::string_view LightController::name() const {
    return "app::LightController";
}

void LightController::setShaderPointLights(engine::resources::Shader *&shader, const std::string &name,
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

void LightController::setShaderDirLights(engine::resources::Shader *&shader, const std::string &name,
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

void LightController::setShaderSpotLights(engine::resources::Shader *&shader, const std::string &name,
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

void LightController::updateLights() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Shader *lightShader = resources->shader("lightPass");
    lightShader->use();
    lightShader->set_int("gPosition", 0);
    lightShader->set_int("gNormal", 1);
    lightShader->set_int("gAlbedoSpec", 2);
    lightShader->set_vec3("viewPos", graphics->camera()
                                             ->Position);
    setShaderPointLights(lightShader, "light", pointLights);
    setShaderDirLights(lightShader, "dirLight", directionalLights);
    setShaderSpotLights(lightShader, "spotLight", spotLights);
}

}
