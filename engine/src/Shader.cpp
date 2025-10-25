#include <glad/glad.h>
#include <engine/resources/Shader.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <cstring>

namespace engine::resources {

void Shader::setup_ubo_matrices(std::vector<glm::mat4> &ubo_matrices) {
    if (!g_uniform_blocks[Matrices]) {
        glGenBuffers(1, &g_uniform_blocks[Matrices]);
        glBindBuffer(GL_UNIFORM_BUFFER, g_uniform_blocks[Matrices]);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo_matrices[0]) * ubo_matrices.size(), NULL, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, Matrices, g_uniform_blocks[Matrices]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, g_uniform_blocks[Matrices]);
    void *ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, ubo_matrices.data(), sizeof(ubo_matrices[0]) * ubo_matrices.size());
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::setup_ubo_lights(UBOLights &ubo_lights) {
    if (!g_uniform_blocks[Lights]) {
        glGenBuffers(1, &g_uniform_blocks[Lights]);
        glBindBuffer(GL_UNIFORM_BUFFER, g_uniform_blocks[Lights]);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo_lights), nullptr, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, Lights, g_uniform_blocks[Lights]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, g_uniform_blocks[Lights]);
    void *ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, &ubo_lights, sizeof(ubo_lights));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::update_lights(UBOLights &ubo_lights) {
    glBindBuffer(GL_UNIFORM_BUFFER, g_uniform_blocks[Lights]);
    size_t offsetLight = sizeof(PointLight);
    size_t offsetPos = sizeof(Light);
    for (size_t i = 0; i < NR_POINT_LIGHTS; i++) {
        glBufferSubData(GL_UNIFORM_BUFFER, i * offsetLight + offsetPos, sizeof(glm::vec3),
                        &ubo_lights.point_lights[i].position);
    }
    offsetLight = sizeof(SpotLight);
    size_t offsetToSpotlights = sizeof(PointLight) * NR_POINT_LIGHTS + sizeof(DirectionalLight) * NR_DIR_LIGHTS;
    for (size_t i = 0; i < NR_SPOT_LIGHTS; i++) {
        glBufferSubData(GL_UNIFORM_BUFFER, offsetToSpotlights + i * offsetLight + offsetPos, sizeof(glm::vec3),
                        &ubo_lights.spot_lights[i].position);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shader::use() const {
    glUseProgram(m_shader_id);
}

void Shader::destroy() const {
    glDeleteProgram(m_shader_id);
}

unsigned Shader::id() const {
    return m_shader_id;
}

void Shader::set_bool(const std::string &name, bool value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform1i, location, static_cast<int>(value));
}

void Shader::set_int(const std::string &name, int value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform1i, location, value);
}

void Shader::set_float(const std::string &name, float value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform1f, location, value);
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform2fv, location, 1, &value[0]);
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform3fv, location, 1, &value[0]);
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniform4fv, location, 1, &value[0]);
}

void Shader::set_mat2(const std::string &name, const glm::mat2 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix2fv, location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string &name, const glm::mat3 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix3fv, location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const {
    uint32_t location = CHECKED_GL_CALL(glGetUniformLocation, m_shader_id, name.c_str());
    CHECKED_GL_CALL(glUniformMatrix4fv, location, 1, GL_FALSE, &mat[0][0]);
}

Shader::Shader(unsigned shader_id, std::string name, std::string source, std::filesystem::path source_path) :
        m_shader_id(shader_id)
        , m_name(std::move(name))
        , m_source(std::move(source))
        , m_source_path(std::move(source_path)) {
}

}
