#ifndef MAINAPP_HPP
#define MAINAPP_HPP

#include <engine/core/App.hpp>
#include <spdlog/spdlog.h>

namespace app {
class MainApp : public engine::core::App {
private:
    void app_setup() override;
};
}
#endif //MAINAPP_HPP
