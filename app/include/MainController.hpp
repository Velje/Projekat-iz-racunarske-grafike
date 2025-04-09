#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <spdlog/spdlog.h>

namespace app {
class MainController : public engine::core::Controller {
public:
    static void update_camera();

    const std::unordered_map<engine::platform::KeyId, engine::graphics::Camera::Movement> &getKeyIdToCameraMovement();

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void drawTerrain();

    void drawSkybox();

    void drawUFO();

    void drawAliens();

    std::string_view name() const override;

};
}
#endif //MAINCONTROLLER_HPP
