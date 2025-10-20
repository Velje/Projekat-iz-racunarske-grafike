#include <glad/glad.h>
#include <engine/resources/Shader.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <cstring>

namespace engine::resources {

enum uniformBlock : size_t {
    Matrices,
    Lights,
    uniformBlockCount,
};
static std::array<uint32_t, uniformBlockCount> uniformBlocks{};

void Shader::setupUBOMatrices(std::vector<glm::mat4> &uboMatrices) {
    if (!uniformBlocks[Matrices]) {
        glGenBuffers(1, &uniformBlocks[Matrices]);
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBlocks[Matrices]);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(uboMatrices[0]) * uboMatrices.size(), NULL, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, Matrices, uniformBlocks[Matrices]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBlocks[Matrices]);
    void *ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, uboMatrices.data(), sizeof(uboMatrices[0]) * uboMatrices.size());
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::setupUBOLights(UBOLights &uboLights) {
    if (!uniformBlocks[Lights]) {
        glGenBuffers(1, &uniformBlocks[Lights]);
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBlocks[Lights]);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(uboLights), nullptr, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, Lights, uniformBlocks[Lights]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBlocks[Lights]);
    void *ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, &uboLights, sizeof(uboLights));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::updateLights(UBOLights &uboLights) {
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBlocks[Lights]);
    size_t offsetLight = sizeof(PointLight);
    size_t offsetPos = sizeof(Light);
    for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
        glBufferSubData(GL_UNIFORM_BUFFER, i * offsetLight + offsetPos, sizeof(glm::vec3),
                        &uboLights.pointLights[i].position);
    }
    offsetLight = sizeof(SpotLight);
    size_t offsetToSpotlights = sizeof(PointLight) * NR_POINT_LIGHTS + sizeof(DirectionalLight) * NR_DIR_LIGHTS;
    for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
        glBufferSubData(GL_UNIFORM_BUFFER, offsetToSpotlights + i * offsetLight + offsetPos, sizeof(glm::vec3),
                        &uboLights.spotLights[i].position);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::use() const {
    glUseProgram(m_shaderId);
}

void Shader::destroy() const {
    glDeleteProgram(m_shaderId);
}

unsigned Shader::id() const {
    return m_shaderId;
}

void Shader::set_bool(const std::string &name, bool value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniform1i, location, static_cast<int>(value));
}

void Shader::set_int(const std::string &name, int value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniform1i, location, value);
}

void Shader::set_float(const std::string &name, float value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniform1f, location, value);
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniform2fv, location, 1, &value[0]);
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniform3fv, location, 1, &value[0]);
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniform4fv, location, 1, &value[0]);
}

void Shader::set_mat2(const std::string &name, const glm::mat2 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix2fv, location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string &name, const glm::mat3 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix3fv, location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shaderId, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix4fv, location, 1, GL_FALSE, &mat[0][0]);
}

Shader::Shader(unsigned shader_id, std::string name, std::string source, std::filesystem::path source_path) :
        m_shaderId(shader_id)
        , m_name(std::move(name))
        , m_source(std::move(source))
        , m_source_path(std::move(source_path)) {
}

}
