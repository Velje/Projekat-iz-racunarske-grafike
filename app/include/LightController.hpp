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

    UBOLights m_ubo_lights;
    LightAttributes m_light_attributes;
    std::vector<glm::vec3> m_initial_pointlight_positions;
    std::vector<glm::vec3> m_initial_spotlight_positions;
    std::vector<glm::vec3> m_initial_dirlight_directions;

    void toggle_point(PointLight &light);

    void update_point(PointLight &&new_light, size_t index);

    void toggle_directional(DirectionalLight &light);

    void update_directional(DirectionalLight &&new_light, size_t index);

    void toggle_spot(SpotLight &light);

    void update_spot(SpotLight &&new_light, size_t index);

    void update_lights();

private:

    void initialize() override;

    void initialize_lights();

    void poll_events() override;

    std::string_view name() const override;

};

}
#endif //LIGHTCONTROLLER_HPP
