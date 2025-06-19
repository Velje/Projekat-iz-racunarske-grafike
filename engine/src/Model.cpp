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

void Model::prepareInstancing(std::vector<glm::mat4>& modelMatrices, size_t count) {
    uint32_t buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4),
                 &modelMatrices[0], GL_STATIC_DRAW);
    for (auto& mesh : m_meshes) {
        mesh.prepareMesh();
    }
}

void Model::drawInstances(Shader*& shader, size_t count) {
    shader->use();
    for (auto& mesh : m_meshes) {
        mesh.drawInstance(shader, count);
    }
}

}
