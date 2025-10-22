#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <spdlog/spdlog.h>

namespace app {

using namespace engine::graphics;

class MainController : public engine::core::Controller {
public:
    static void update_camera();

    static const std::unordered_set<engine::platform::KeyId> &get_key_controls();

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    std::string_view name() const override;

    void deferred_render();

    void geometry_pass();

    void draw_earth();

    void draw_platform();

    void draw_terrain();

    void draw_ufo();

    void draw_ufo_normals();

    void draw_ufo_2();

    void light_pass();

    void draw_skybox();

    static void update_model_position(glm::mat4 &model, Camera::Movement &direction);

};
}
#endif //MAINCONTROLLER_HPP
