#ifndef LIGHTCONTROLLER_HPP
#define LIGHTCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>
#include <array>
#include <glm/glm.hpp>

namespace app {

static glm::vec3 lightColor(1.0f);
static glm::vec3 ambientStrength(1.0f), diffuseStrength(1.0f), specularStrength(1.0f);
static float shininess = 2048.0f;
const static size_t NR_POINT_LIGHTS = 64;
const static size_t NR_DIR_LIGHTS = 4;
const static size_t NR_SPOT_LIGHTS = 16;

class Light {
public:
    glm::vec3 color;
    glm::vec3 ambientStrength;
    glm::vec3 diffuseStrength;
    glm::vec3 specularStrength;
    float constant;
    float linear;
    float quadratic;
    float shininess;
    bool enabled = true;
};

class PointLight : public Light {
public:
    glm::vec3 position;
};

class DirectionalLight : public Light {
public:
    glm::vec3 direction;
};

class SpotLight : public Light {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
};

class LightController : public engine::core::Controller {
public:

    static std::array<PointLight, NR_POINT_LIGHTS> &getPointLights();

    static std::array<DirectionalLight, NR_DIR_LIGHTS> &getDirectionalLights();

    static std::array<SpotLight, NR_SPOT_LIGHTS> &getSpotLights();

    static void updatePoint(PointLight newLight, size_t index);

    static void updateDirectional(DirectionalLight newLight, size_t index);

    static void updateSpot(SpotLight newLight, size_t index);

    static void togglePoint(PointLight &light);

    static void toggleDirectional(DirectionalLight &light);

    static void toggleSpot(SpotLight &light);

    void updateLights();

private:

    void initialize() override;

    void poll_events() override;

    void setShaderPointLights(engine::resources::Shader *&shader, const std::string &name,
                              std::array<PointLight, NR_POINT_LIGHTS> &lights);

    void setShaderDirLights(engine::resources::Shader *&shader, const std::string &name,
                            std::array<DirectionalLight, NR_DIR_LIGHTS> &lights);

    void setShaderSpotLights(engine::resources::Shader *&shader, const std::string &name,
                             std::array<SpotLight, NR_SPOT_LIGHTS> &lights);

    std::string_view name() const override;

};

}
#endif //LIGHTCONTROLLER_HPP
