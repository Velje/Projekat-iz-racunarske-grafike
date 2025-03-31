#include <LightController.hpp>

namespace app {

static std::list<PointLight> pointLights;
static std::list<DirectionalLight> directionalLights;
static std::list<SpotLight> spotLights;

void LightController::initialize() {
    
}

void LightController::poll_events() {

}

void LightController::togglePoint(PointLight &light) {
    light.enabled = !light.enabled;
}

void LightController::addPoint(PointLight light) {
    pointLights.push_back(light);
}

std::list<PointLight> &LightController::getPointLights() {
    return pointLights;
}

void LightController::toggleDirectional(DirectionalLight &light) {
    light.enabled = !light.enabled;
}

void LightController::addDirectional(DirectionalLight light) {
    directionalLights.push_back(light);
}

std::list<DirectionalLight> &LightController::getDirectionalLights() {
    return directionalLights;
}

void LightController::toggleSpot(SpotLight &light) {
    light.enabled = !light.enabled;
}

void LightController::addSpot(SpotLight light) {
    spotLights.push_back(light);
}

std::list<SpotLight> &LightController::getSpotLights() {
    return spotLights;
}

std::string_view LightController::name() const {
    return "app::LightController";
}


}
