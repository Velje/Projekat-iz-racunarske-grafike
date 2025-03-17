#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {
class MainController : public engine::core::Controller {
private:
    void initialize() override;

    bool loop() override;

    void draw() override;

    void drawBackpack();

    std::string_view name() const override;
};
}
#endif //MAINCONTROLLER_HPP
