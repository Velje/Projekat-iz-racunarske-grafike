#include <LightController.hpp>
#include <GUIController.hpp>

namespace app {

static UBOLights uboLights;
static LightAttributes lightAttributes;

LightAttributes *LightController::getLightAttributesAddress() {
    return &lightAttributes;
}

LightAttributes &LightController::getLightAttributesReference() {
    return lightAttributes;
}

UBOLights *LightController::getUBOLightsAddress() {
    return &uboLights;
}

UBOLights &LightController::getUBOLightsReference() {
    return uboLights;
}

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
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
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
        updatePoint(PointLight(Light(lightColor), glm::vec3(lights[i].first, 4.0f, 200.0f + lights[i].second)), i);
    }
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < NR_DIR_LIGHTS; i++) {
        auto flipX = i % 2 ? 1.0f : -1.0f;
        if (i < 2) {
            updateDirectional(DirectionalLight(Light(lightColor), glm::vec3(flipX * 1.0f, -1.0f, 1.0f)), i);
        } else {
            updateDirectional(DirectionalLight(Light(lightColor), glm::vec3(flipX * 1.0f, -1.0f, -1.0f)), i);
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
        updateSpot(SpotLight(Light(lightColor), glm::vec3(50.0f * cos(angle), 145.0f, 200.0f - 50.0f * sin(angle)),
                             glm::vec3(0.0f, -1.0f, 0.0f), cos(glm::radians(1.0f)), cos(glm::radians(3.0f))), i);
    }
}

void LightController::poll_events() {
    auto guiController = engine::core::Controller::get<GUIController>();
    if (guiController->is_enabled()) {
        set_enable(!is_enabled());
    }
}

void LightController::updatePoint(PointLight &&newLight, size_t index) {
    RG_GUARANTEE(index < NR_POINT_LIGHTS, "point light index out of range");
    uboLights.pointLights[index] = newLight;
}

void LightController::togglePoint(PointLight &light) {
    light.base
         .enabled = !light.base
                          .enabled;
}

void LightController::updateDirectional(DirectionalLight &&newLight, size_t index) {
    RG_GUARANTEE(index < NR_DIR_LIGHTS, "dir light index out of range");
    uboLights.dirLights[index] = newLight;
}

void LightController::toggleDirectional(DirectionalLight &light) {
    light.base
         .enabled = !light.base
                          .enabled;
}

void LightController::toggleSpot(SpotLight &light) {
    light.base
         .enabled = !light.base
                          .enabled;
}

void LightController::updateSpot(SpotLight &&newLight, size_t index) {
    RG_GUARANTEE(index < NR_SPOT_LIGHTS, "spot light index out of range");
    uboLights.spotLights[index] = newLight;
}

std::string_view LightController::name() const {
    return "app::LightController";
}

void LightController::updateLights() {
    auto resources = engine::core::Controller::get<ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    Shader *lightShader = resources->shader("lightPass");
    lightShader->use();
    lightShader->set_int("gPosition", 0);
    lightShader->set_int("gNormal", 1);
    lightShader->set_int("gAlbedoSpec", 2);
    lightShader->set_vec3("viewPos", graphics->camera()
                                             ->Position);
    lightShader->set_float("exposure", lightAttributes.exposure);
    lightShader->set_float("gamma", lightAttributes.gamma);
    Shader::setupUBOLights(uboLights);
}

}
