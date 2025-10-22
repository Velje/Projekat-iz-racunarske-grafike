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

    static LightAttributes &get_light_attributes_reference();

    static UBOLights &get_ubo_lights_reference();

    static std::vector<std::pair<float, float>> &get_pointlight_positions();

    static void toggle_point(PointLight &light);

    static void update_point(PointLight &&new_light, size_t index);

    static void toggle_directional(DirectionalLight &light);

    static void update_directional(DirectionalLight &&new_light, size_t index);

    static void toggle_spot(SpotLight &light);

    static void update_spot(SpotLight &&new_light, size_t index);

    static void update_lights();

private:

    void initialize() override;

    void poll_events() override;

    std::string_view name() const override;

};

}
#endif //LIGHTCONTROLLER_HPP
