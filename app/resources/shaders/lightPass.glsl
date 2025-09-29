//#shader vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

//#shader fragment
#version 460 core

out vec4 FragColor;
in vec3 FragPos;
in vec2 TexCoords;

struct Light {
    vec3 color; // 16
    vec3 ambientStrength; //16
    vec3 diffuseStrength; // 16
    vec3 specularStrength; // 16
    vec4 attenuation; // 16
    bool enabled; // 4 + 12
}; // 96B

struct PointLight {
    Light base; // 96
    vec3 position; // 16
}; // 112B

struct DirLight {
    Light base; // 96
    vec3 direction; // 16
}; // 112B

struct SpotLight {
    Light base; // 96
    vec3 position; // 16
    vec3 direction; // 16
    float cutOff; // 4
    float outcutOff; // 4 + 8
}; // 144B

#define NR_POINT_LIGHTS 64
#define NR_DIR_LIGHTS 4
#define NR_SPOT_LIGHTS 64

layout (std140, binding = 1) uniform Lights {
    PointLight light[NR_POINT_LIGHTS]; // 112B * 256
    DirLight dirLight[NR_DIR_LIGHTS]; // 112B * 4
    SpotLight spotLight[NR_SPOT_LIGHTS]; // 144B * 64
}; // 38336B

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform float exposure, gamma;

float calculateDiffuse(vec3 modelNormal, vec3 lightDir) {
    return max(dot(modelNormal, lightDir), 0.0f);
}

float calculateSpecular(vec3 viewDir, vec3 reflectDir, float shininess) {
    return pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
}

void main() {
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 modelNormal = normalize(texture(gNormal, TexCoords).rgb * 2.0f - 1.0f);
    vec4 albedoSpec = texture(gAlbedoSpec, TexCoords);
    vec3 modelDiffuse = albedoSpec.rgb;
    float modelSpecular = albedoSpec.a;
    vec3 result = vec3(0.0f);
    for (uint i = 0; i < NR_POINT_LIGHTS; i++) {
        if (light[i].base.enabled) {
            vec3 lightDir = normalize(light[i].position - FragPos);
            vec3 reflectDir = reflect(-lightDir, modelNormal);
            vec3 ambient = light[i].base.ambientStrength * light[i].base.color * modelDiffuse;
            vec3 diffuse = light[i].base.diffuseStrength * calculateDiffuse(modelNormal, lightDir)
            * light[i].base.color * modelDiffuse;
            vec3 specular = light[i].base.specularStrength * calculateSpecular(viewDir, reflectDir, light[i].base.attenuation.w)
            * light[i].base.color * modelSpecular;
            float distance = length(light[i].position - FragPos);
            if (distance <= 10.0f) {
                result += ambient + (diffuse + specular) /
                (light[i].base.attenuation.x + light[i].base.attenuation.y * distance
                + light[i].base.attenuation.z * distance * distance);
            }
        }
    }
    for (uint i = 0; i < NR_DIR_LIGHTS; i++) {
        if (dirLight[i].base.enabled) {
            vec3 lightDir = normalize(-dirLight[i].direction);
            vec3 reflectDir = reflect(-lightDir, modelNormal);
            vec3 ambient = dirLight[i].base.ambientStrength * dirLight[i].base.color * modelDiffuse;
            vec3 diffuse = dirLight[i].base.diffuseStrength * calculateDiffuse(modelNormal, lightDir)
            * dirLight[i].base.color * modelDiffuse;
            vec3 specular = dirLight[i].base.specularStrength *
            calculateSpecular(viewDir, reflectDir, dirLight[i].base.attenuation.w) *
            dirLight[i].base.color * modelSpecular;
            result += ambient + (diffuse + specular) / (dirLight[i].base.attenuation.x +
            dirLight[i].base.attenuation.y + dirLight[i].base.attenuation.z);
        }
    }
    for (uint i = 0; i < NR_SPOT_LIGHTS; i++) {
        if (spotLight[i].base.enabled) {
            vec3 lightDir = normalize(spotLight[i].position - FragPos);
            float theta = dot(lightDir, normalize(-spotLight[i].direction));
            float epsilon = spotLight[i].cutOff - spotLight[i].outcutOff;
            float intensity = clamp((theta - spotLight[i].outcutOff) / epsilon, 0.0f, 1.0f);
            vec3 reflectDir = reflect(-lightDir, modelNormal);
            vec3 ambient = intensity * spotLight[i].base.ambientStrength * spotLight[i].base.color * modelDiffuse;
            vec3 diffuse = intensity * spotLight[i].base.diffuseStrength * calculateDiffuse(modelNormal, lightDir) *
            spotLight[i].base.color * modelDiffuse;
            vec3 specular = intensity * spotLight[i].base.specularStrength * calculateSpecular(viewDir, reflectDir, spotLight[i].base.attenuation.w)
            * spotLight[i].base.color * modelSpecular;
            float distance = length(spotLight[i].position - FragPos);
            if (distance <= 146.0f) {
                result += ambient + (diffuse + specular) /
                (spotLight[i].base.attenuation.x + spotLight[i].base.attenuation.y * distance +
                spotLight[i].base.attenuation.z * distance * distance);
            }
        }
    }
    result = vec3(1.0f) - exp(-result * exposure);
    result = pow(result, vec3(gamma));
    FragColor = vec4(result, 1.0);

}
