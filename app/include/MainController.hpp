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

    static const std::unordered_set<engine::platform::KeyId> &getKeyControls();

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    std::string_view name() const override;

    void deferredRender();

    void geometryPass();

    void drawEarth();

    void drawPlatform();

    void drawTerrain();

    void drawUFO();

    void drawUFONormals();

    void drawUFO2();

    void lightPass();

    void drawSkybox();

    static void updateModelPosition(glm::mat4 &model, Camera::Movement &direction);

};
}
#endif //MAINCONTROLLER_HPP
