#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace app {

class GUIController : public engine::core::Controller {
private:
    void initialize() override;

    void poll_events() override;

    void draw() override;

    std::string_view name() const override;
};
}

#endif //GUICONTROLLER_HPP
