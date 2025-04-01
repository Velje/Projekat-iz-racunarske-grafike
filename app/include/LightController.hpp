#ifndef LIGHTCONTROLLER_HPP
#define LIGHTCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>
#include <array>
#include <glm/glm.hpp>

namespace app {

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
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
};

const static size_t NR_POINT_LIGHTS = 2;
const static size_t NR_DIR_LIGHTS = 2;
const static size_t NR_SPOT_LIGHTS = 2;

class LightController : public engine::core::Controller {
public:

    const std::array<PointLight, NR_POINT_LIGHTS> &getPointLights();

    const std::array<DirectionalLight, NR_DIR_LIGHTS> &getDirectionalLights();

    const std::array<SpotLight, NR_SPOT_LIGHTS> &getSpotLights();

    void updatePoint(PointLight newLight, size_t index);

    void setShaderPointLights(engine::resources::Shader *shader, const std::string &name,
                              std::array<PointLight, NR_POINT_LIGHTS> &lights);

    void updateDirectional(DirectionalLight newLight, size_t index);

    void setShaderDirLights(engine::resources::Shader *shader, const std::string &name,
                            std::array<DirectionalLight, NR_DIR_LIGHTS> &lights);

    void updateSpot(SpotLight newLight, size_t index);

    void setShaderSpotLights(engine::resources::Shader *shader, const std::string &name,
                             std::array<SpotLight, NR_SPOT_LIGHTS> &lights);

    void togglePoint(PointLight &light);

    void toggleDirectional(DirectionalLight &light);

    void toggleSpot(SpotLight &light);

private:

    void initialize() override;

    void poll_events() override;

    std::string_view name() const override;

};

}
#endif //LIGHTCONTROLLER_HPP
