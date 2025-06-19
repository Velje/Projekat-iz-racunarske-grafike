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
        glBindVertexArray(mesh.getVAO());
        std::size_t v4s = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4*v4s, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4*v4s, (void*)(1*v4s));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4*v4s, (void*)(2*v4s));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4*v4s, (void*)(3*v4s));
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }
}

void Model::drawInstances(const Shader* shader, size_t count) {
    shader->use();
    for (auto& mesh : m_meshes) {
        std::unordered_map<std::string_view, uint32_t> counts;
        std::string uniform_name;
        uniform_name.reserve(32);
        auto textures = mesh.getTextures();
        for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            const auto &texture_type = Texture::uniform_name_convention(textures[i]->type());
            uniform_name.append(texture_type);
            const auto count = (counts[texture_type] += 1);
            uniform_name.append(std::to_string(count));
            shader->set_int(uniform_name, i);
            glBindTexture(GL_TEXTURE_2D, textures[i]->id());
            uniform_name.clear();
        }
        glBindVertexArray(mesh.getVAO());
        glDrawElementsInstanced(GL_TRIANGLES,
                                mesh.getNumIndices(), GL_UNSIGNED_INT, 0, count);
    }
}

}
