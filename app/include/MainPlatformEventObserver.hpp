#ifndef MAINPLATFORMEVENTOBSERVER_HPP
#define MAINPLATFORMEVENTOBSERVER_HPP

#include <engine/platform/PlatformEventObserver.hpp>
#include <engine/util/Configuration.hpp>

namespace app {

class MainController;

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {

private:

    MainController *m_main_controller{engine::core::Controller::get<MainController>()};

    void on_mouse_move(engine::platform::MousePosition position) override;

    void on_scroll(engine::platform::MousePosition position) override;

    void on_key(engine::platform::Key key) override;

};
}

#endif //MAINPLATFORMEVENTOBSERVER_HPP
