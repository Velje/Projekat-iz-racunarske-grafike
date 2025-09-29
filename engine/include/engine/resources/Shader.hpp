/**
 * @file Shader.hpp
 * @brief Defines the Shader class that serves as an abstraction over OpenGL shaders.
 */

#ifndef MATF_RG_PROJECT_SHADER_HPP
#define MATF_RG_PROJECT_SHADER_HPP

#include <engine/util/Utils.hpp>
#include <string>
#include <glm/glm.hpp>

namespace engine::resources {
static const size_t NR_POINT_LIGHTS = 64;
static const size_t NR_DIR_LIGHTS = 4;
static const size_t NR_SPOT_LIGHTS = 64;
static const size_t totalLightCount = NR_POINT_LIGHTS + NR_DIR_LIGHTS + NR_SPOT_LIGHTS;

// Base layout for all lights
struct alignas(16) Light {
    alignas(16) glm::vec3 color{1.0f};
    alignas(16) glm::vec3 ambientStrength{1.0f};
    alignas(16) glm::vec3 diffuseStrength{1.0f};
    alignas(16) glm::vec3 specularStrength{1.0f};
    alignas(16) glm::vec4 attenuation{1.0f, 0.0f, 0.0f, 2048.0f};
    alignas(4) bool enabled;
};

// Point light
struct alignas(16) PointLight {
    Light base;
    alignas(16) glm::vec3 position;
};

// Directional light
struct alignas(16) DirectionalLight {
    Light base;
    alignas(16) glm::vec3 direction;
};

// Spot light
struct alignas(16) SpotLight {
    Light base;
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;
    alignas(4) float cutOff;
    alignas(4) float outerCutOff;
};

struct alignas(16) UBOLights {
    std::array<PointLight, NR_POINT_LIGHTS> pointLights;
    std::array<DirectionalLight, NR_DIR_LIGHTS> dirLights;
    std::array<SpotLight, NR_SPOT_LIGHTS> spotLights;
};

using ShaderName = std::string;

/**
* @enum ShaderType
* @brief The type of the shader.
*/
enum class ShaderType {
    Vertex, Fragment, Geometry
};

/**
* @brief Converts a @ref ShaderType to a string.
* @param type The type of the shader.
* @returns The string representation of the shader type.
*/
std::string_view to_string(ShaderType type);

/**
* @class Shader
* @brief Represents a linked shader program object within the OpenGL context.
*/
class Shader {
    friend class ShaderCompiler;

public:
    /**
    * @brief Binds the shader program.
    */
    void use() const;

    /**
    * @brief Returns the OpenGL ID of the shader program.
    * @returns The OpenGL ID of the shader program.
    */
    unsigned id() const;

    /**
    * @brief Sets a boolean uniform value.
    * @param name The name of the uniform.
    * @param value The value to set.
    */
    void set_bool(const std::string &name, bool value) const;

    /**
    * @brief Sets an integer uniform value.
    * @param name The name of the uniform.
    * @param value The value to set.
    */
    void set_int(const std::string &name, int value) const;

    /**
    * @brief Sets a float uniform value.
    * @param name The name of the uniform.
    * @param value The value to set.
    */
    void set_float(const std::string &name, float value) const;

    /**
    * @brief Sets a 2D vector uniform value.
    * @param name The name of the uniform.
    * @param value The value to set.
    */
    void set_vec2(const std::string &name, const glm::vec2 &value) const;

    /**
    * @brief Sets a 3D vector uniform value.
    * @param name The name of the uniform.
    * @param value The value to set.
    */
    void set_vec3(const std::string &name, const glm::vec3 &value) const;

    /**
    * @brief Sets a 4D vector uniform value.
    * @param name The name of the uniform.
    * @param value The value to set.
    */
    void set_vec4(const std::string &name, const glm::vec4 &value) const;

    /**
    * @brief Sets a 2x2 matrix uniform value.
    * @param name The name of the uniform.
    * @param mat The value to set.
    */
    void set_mat2(const std::string &name, const glm::mat2 &mat) const;

    /**
    * @brief Sets a 3x3 matrix uniform value.
    * @param name The name of the uniform.
    * @param mat The value to set.
    */
    void set_mat3(const std::string &name, const glm::mat3 &mat) const;

    /**
    * @brief Sets a 4x4 matrix uniform value.
    * @param name The name of the uniform.
    * @param mat The value to set.
    */
    void set_mat4(const std::string &name, const glm::mat4 &mat) const;

    /**
    * @brief Returns the name of the shader program by which it can be referenced using the @ref engine::resources::ResourcesController::shader function.
    * @returns The name of the shader.
    */
    const std::string &name() const;

    /**
    * @brief Returns the source code of the shader program.
    * @returns The source code of the shader.
    */
    const std::string &source() const;

    /**
    * @brief Returns the path to the source file from which the shader program was compiled.
    * @returns The path to the source file of the shader program.
    */
    const std::filesystem::path &source_path() const;

    static void setupUBOMatrices(std::vector<glm::mat4> &uboMatrices);

    static void setupUBOLights(UBOLights &uboLights);

private:
    /**
    * @brief Constructs a Shader object.
    * @param shader_id The OpenGL ID of the shader program.
    * @param name The name of the shader program.
    * @param source The source code of the shader program.
    * @param source_path The path to the source file from which the shader program was compiled.
    */
    Shader(unsigned shader_id, std::string name, std::string source,
           std::filesystem::path source_path = "");

    /**
    * @brief Destroys the shader program in the OpenGL context.
    */
    void destroy() const;

    /**
    * @brief The OpenGL ID of the shader program.
    */
    unsigned m_shaderId;

    /**
    * @brief The name of the shader program
    */
    std::string m_name;
    std::string m_source;
    std::filesystem::path m_source_path;
};
} // namespace engine

#endif//MATF_RG_PROJECT_SHADER_HPP
