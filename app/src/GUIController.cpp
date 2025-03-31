#include <GUIController.hpp>
#include <EventController.hpp>

namespace app {

static glm::vec3 lightColor(1.0f);
static glm::vec3 ambientStrength, diffuseStrength(4.0f), specularStrength(10.0f);
static float shininess = 128.0f;

void GUIController::initialize() {
    set_enable(false);
}

void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto eventController = engine::core::Controller::get<EventController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed
            ) {
        eventController->set_enable(!eventController->is_enabled());
        set_enable(!is_enabled());
    }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto camera = graphics->camera();
    graphics->begin_gui();
    ImGui::Begin("Camera info");
    ImGui::Text("Camera position: (%f, %f, %f)", camera->Position
                                                       .x, camera->Position
                                                                 .y, camera->Position
                                                                           .z);
    ImGui::Text("Mouse position: %f %f", platform->mouse()
                                                 .x, platform->mouse()
                                                             .y);
    auto deltaTime = platform->dt();
    ImGui::Text("Frames per second: %f", 1 / deltaTime);
    ImGui::Text("Frame difference: %fms", deltaTime * 1000);
    ImGui::Text("Point light");
    ImGui::ColorEdit3("color", glm::value_ptr(lightColor));
    ImGui::DragFloat3("ambientStrength", glm::value_ptr(ambientStrength), 0.01, 0.0f, 100.0f);
    ImGui::DragFloat3("diffuseStrength", glm::value_ptr(diffuseStrength), 0.01, 0.0f, 100.0f);
    ImGui::DragFloat3("specularStrength", glm::value_ptr(specularStrength), 0.01, 0.0f, 100.0f);
    ImGui::DragFloat("shininess", &shininess, 0.01f, 0.0f, 2048.0f);
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_vec3("light.color", lightColor);
    shader->set_vec3("light.ambientStrength", ambientStrength);
    shader->set_vec3("light.diffuseStrength", diffuseStrength);
    shader->set_vec3("light.specularStrength", specularStrength);
    shader->set_float("light.shininess", shininess);
    ImGui::End();
    graphics->end_gui();
}

std::string_view GUIController::name() const {
    return "app::GUIController";
}

}


