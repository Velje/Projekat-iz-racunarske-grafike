#include <GUIController.hpp>
#include <EventController.hpp>
#include <LightController.hpp>

namespace app {

static UBOLights &uboLights = LightController::getUBOLightsReference();
static LightAttributes &lightAttributes = LightController::getLightAttributesReference();
static int pointSelector = 0, dirSelector = 0, spotSelector = 0;

void GUIController::initialize() {
    set_enable(false);
}

void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto eventController = engine::core::Controller::get<EventController>();
    if (platform->key(engine::platform::KeyId::KEY_F2)
                .state() == engine::platform::Key::State::JustPressed
            ) {
        auto actionStart = platform->getGlfwTime();
        auto actionEnd = platform->getGlfwTime();
        auto eventStart = platform->getGlfwTime();
        eventController->set_enable(!eventController->is_enabled());
        Shader::setupUBOLights(uboLights);
        set_enable(!is_enabled());
        auto eventEnd = platform->getGlfwTime();
        EventController::instaLog(
                Action(Actions::PRESS, actionEnd - actionStart, EventA::KEY, eventEnd - eventStart,
                       EventB::GUI_TOGGLE));
    }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = graphics->camera();
    graphics->begin_gui();
    ImGui::Begin("GUI");
    ImGui::Text("Camera info");
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
    ImGui::Text("Light control");
    ImGui::InputInt("Pointlight index", &pointSelector);
    if (pointSelector >= 0 && pointSelector < NR_POINT_LIGHTS) {
        auto &pointLight = uboLights.pointLights[pointSelector];
        if (ImGui::RadioButton("Enabled1", pointLight.base
                                                     .enabled)) {
            pointLight.base
                      .enabled = !pointLight.base
                                            .enabled;
        }
        ImGui::ColorEdit3("Point light color", glm::value_ptr(pointLight.base
                                                                        .color));
        ImGui::DragFloat3("ambientStrength1", glm::value_ptr(pointLight.base
                                                                       .ambientStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("diffuseStrength1", glm::value_ptr(pointLight.base
                                                                       .diffuseStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("specularStrength1", glm::value_ptr(pointLight.base
                                                                        .specularStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat("shininess1", &pointLight.base
                                                  .shininess, 1.0f, 0.0f, 100.0f);
    }

    ImGui::InputInt("Dirlight index", &dirSelector);
    if (dirSelector >= 0 && dirSelector < NR_DIR_LIGHTS) {
        auto &dirLight = uboLights.dirLights[dirSelector];
        if (ImGui::RadioButton("Enabled2", dirLight.base
                                                   .enabled)) {
            dirLight.base
                    .enabled = !dirLight.base
                                        .enabled;
        }
        ImGui::ColorEdit3("Directional light color", glm::value_ptr(dirLight.base
                                                                            .color));
        ImGui::DragFloat3("ambientStrength2", glm::value_ptr(dirLight.base
                                                                     .ambientStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("diffuseStrength2", glm::value_ptr(dirLight.base
                                                                     .diffuseStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("specularStrength2", glm::value_ptr(dirLight.base
                                                                      .specularStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat("shininess2", &dirLight.base
                                                .shininess, 1.0f, 0.0f, 2048.0f);
    }

    ImGui::InputInt("Spotlight index", &spotSelector);
    if (spotSelector >= 0 && spotSelector < NR_SPOT_LIGHTS) {
        auto &spotLight = uboLights.spotLights[spotSelector];
        if (ImGui::RadioButton("Enabled3", spotLight.base
                                                    .enabled)) {
            spotLight.base
                     .enabled = !spotLight.base
                                          .enabled;
        }
        ImGui::ColorEdit3("Spotlight color", glm::value_ptr(spotLight.base
                                                                     .color));
        ImGui::DragFloat3("ambientStrength3", glm::value_ptr(spotLight.base
                                                                      .ambientStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("diffuseStrength3", glm::value_ptr(spotLight.base
                                                                      .diffuseStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat3("specularStrength3", glm::value_ptr(spotLight.base
                                                                       .specularStrength), 0.5f, 0.0f, 100.0f);
        ImGui::DragFloat("shininess3", &spotLight.base
                                                 .shininess, 1.0f, 0.0f, 2048.0f);
    }
    ImGui::DragFloat("exposure", &lightAttributes.exposure, 0.25f, 0.0f, 100.0f);
    ImGui::DragFloat("gamma", &lightAttributes.gamma, 0.25f, 0.0f, 100.0f);
    ImGui::End();
    graphics->end_gui();
}

std::string_view GUIController::name() const {
    return "app::GUIController";
}

}
