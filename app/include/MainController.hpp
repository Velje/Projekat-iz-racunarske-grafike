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
private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void update_camera();

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void drawBackpack();

    void drawSkybox();

    void drawFloor();

    std::string_view name() const override;

};
}
#endif //MAINCONTROLLER_HPP
