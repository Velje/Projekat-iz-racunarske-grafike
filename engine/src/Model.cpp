#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>
#include <glad/glad.h>

namespace engine::resources {

void Model::draw(const Shader *shader) {
    shader->use();
    for (auto &mesh: m_meshes) {
        mesh.draw(shader);
    }
}

void Model::destroy() {
    for (auto &mesh: m_meshes) {
        mesh.destroy();
    }
}

void Model::prepare_instancing_data(std::vector<glm::mat4> &model_matrices) {
    for (auto &mesh: m_meshes) {
        mesh.prepare_model_data(model_matrices);
    }
}

void Model::draw_instances(const Shader *shader, const size_t count) {
    shader->use();
    for (auto &mesh: m_meshes) {
        mesh.draw_instances(shader, count);
    }
}

}
