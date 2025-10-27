#include <MainController.hpp>

namespace app {

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    float actionStart = m_main_controller->m_platform_controller
                                         ->get_glfw_time();
    auto camera = m_main_controller->m_graphics_controller
                                   ->camera();
    if (!m_main_controller->m_gui_controller
                          ->is_enabled()) {
        float actionEnd = m_main_controller->m_platform_controller
                                           ->get_glfw_time();
        float eventStart = m_main_controller->m_platform_controller
                                            ->get_glfw_time();
        camera->rotate_camera(position.dx, position.dy);
        float eventEnd = m_main_controller->m_platform_controller
                                          ->get_glfw_time();
        m_main_controller->m_event_controller
                         ->insta_log(Action(Actions::MOVE, actionEnd - actionStart, EventA::MOUSE,
                                            eventEnd - eventStart, EventB::CAMERA_ROTATION));
    }

}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    float actionStart = m_main_controller->m_platform_controller
                                         ->get_glfw_time();
    auto camera = m_main_controller->m_graphics_controller
                                   ->camera();
    if (!m_main_controller->m_gui_controller
                          ->is_enabled()) {
        float actionEnd = m_main_controller->m_platform_controller
                                           ->get_glfw_time();
        float eventStart = m_main_controller->m_platform_controller
                                            ->get_glfw_time();
        camera->zoom(position.scroll);
        float eventEnd = m_main_controller->m_platform_controller
                                          ->get_glfw_time();
        m_main_controller->m_event_controller
                         ->insta_log(Action(Actions::SCROLL, actionEnd - actionStart, EventA::MOUSE,
                                            eventEnd - eventStart, EventB::CAMERA_ZOOM));
    }
}

void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    float actionStart = m_main_controller->m_platform_controller
                                         ->get_glfw_time();
    auto &controls = MainController::get_key_controls();
    if (!m_main_controller->m_gui_controller
                          ->is_enabled()) {
        float actionEnd = m_main_controller->m_platform_controller
                                           ->get_glfw_time();
        float eventStart = m_main_controller->m_platform_controller
                                            ->get_glfw_time();
        if (controls.find(key.id()) == end(controls)) {
            spdlog::info("{0} is not supported.", key.name());
            float eventEnd = m_main_controller->m_platform_controller
                                              ->get_glfw_time();
            m_main_controller->m_event_controller
                             ->insta_log(Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY,
                                                eventEnd - eventStart, EventB::NOTHING_B));
        }
    }
}

}
