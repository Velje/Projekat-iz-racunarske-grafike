#include <engine/core/Engine.hpp>
#include <MainApp.hpp>

int main(int argc, char **argv) {
    spdlog::info("Running MainApp");
    return std::make_unique<app::MainApp>()->run(argc, argv);
}
