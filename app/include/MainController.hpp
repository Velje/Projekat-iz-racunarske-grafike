#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {
class MainController : public engine::core::Controller {
private:
    void initialize() override;

    std::string_view name() const override;
};
}
#endif //MAINCONTROLLER_HPP
