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

void Model::drawInstances(Shader *&shader, std::vector<glm::mat4> &modelMatrices) {
    shader->use();
    for (auto &mesh: m_meshes) {
        mesh.drawInstances(shader, modelMatrices);
    }
}

}
