#include <engine/core/Engine.hpp>

class MyApp : public engine::core::App {

public:
    void app_setup() override;
};

void MyApp::app_setup() {
    spdlog::info("Hello, setup!");
}

int main(int argc, char **argv) {
    return std::make_unique<MyApp>()->run(argc, argv);
}
