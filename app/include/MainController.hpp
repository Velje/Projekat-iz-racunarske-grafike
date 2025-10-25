#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <spdlog/spdlog.h>
#include <MainPlatformEventObserver.hpp>
#include <EventController.hpp>
#include <LightController.hpp>
#include <GUIController.hpp>

namespace app {

static const size_t NR_UFO2_MODELS = 480, NR_UBO_MATRICES = 2;

using namespace engine::graphics;

class MainController : public engine::core::Controller {
    friend class MainPlatformEventObserver;

    friend class GUIController;

    friend class LightController;

    friend class EventController;

public:

    static const std::unordered_set<engine::platform::KeyId> &get_key_controls();

private:

    LightController *m_light_controller;
    EventController *m_event_controller;
    GUIController *m_gui_controller;
    engine::resources::ResourcesController *m_resources_controller;
    GraphicsController *m_graphics_controller;
    engine::platform::PlatformController *m_platform_controller;

    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void update_camera();

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

    void update_model_position(glm::mat4 &model, Camera::Movement &direction);

};
}
#endif //MAINCONTROLLER_HPP
