#ifndef LIGHTCONTROLLER_HPP
#define LIGHTCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Shader.hpp>
#include <array>
#include <glm/glm.hpp>

namespace app {

struct LightAttributes {
    float gamma{2.2f};
    float exposure{1.0f};
};

using namespace engine::resources;

class LightController : public engine::core::Controller {
public:

    static LightAttributes &getLightAttributesReference();

    static UBOLights &getUBOLightsReference();

    static std::vector<std::pair<float, float>> &getLightPositions();

    static void togglePoint(PointLight &light);

    static void updatePoint(PointLight &&newLight, size_t index);

    static void toggleDirectional(DirectionalLight &light);

    static void updateDirectional(DirectionalLight &&newLight, size_t index);

    static void toggleSpot(SpotLight &light);

    static void updateSpot(SpotLight &&newLight, size_t index);

    static void updateLights();

private:

    void initialize() override;

    void poll_events() override;

    std::string_view name() const override;

};

}
#endif //LIGHTCONTROLLER_HPP
