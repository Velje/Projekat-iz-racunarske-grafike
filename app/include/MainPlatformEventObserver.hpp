#ifndef MAINPLATFORMEVENTOBSERVER_HPP
#define MAINPLATFORMEVENTOBSERVER_HPP

#include <engine/platform/PlatformController.hpp>

namespace app {
class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    void on_mouse_move(engine::platform::MousePosition position) override;
};
}

#endif //MAINPLATFORMEVENTOBSERVER_HPP
