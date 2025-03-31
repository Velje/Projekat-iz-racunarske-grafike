#ifndef LIGHTCONTROLLER_HPP
#define LIGHTCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <list>
#include <glm/glm.hpp>

namespace app {

class PointLight {
public:
    bool enabled;
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 ambientStrength;
    glm::vec3 diffuseStrength;
    glm::vec3 specularStrength;
    float constant;
    float linear;
    float quadratic;
    float shininess;
};

class DirectionalLight : public PointLight {
public:
    glm::vec3 direction;
};

class SpotLight : public PointLight {
public:
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
};

class LightController : public engine::core::Controller {
public:

    static std::list<PointLight> &getPointLights();

    static std::list<DirectionalLight> &getDirectionalLights();

    static std::list<SpotLight> &getSpotLights();

    static void addPoint(PointLight light);

    static void addDirectional(DirectionalLight light);

    static void addSpot(SpotLight light);

    static void togglePoint(PointLight &light);

    static void toggleDirectional(DirectionalLight &light);

    static void toggleSpot(SpotLight &light);

private:

    void initialize() override;

    void poll_events() override;

    std::string_view name() const override;

};

}
#endif //LIGHTCONTROLLER_HPP
