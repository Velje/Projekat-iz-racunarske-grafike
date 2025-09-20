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
in vec3 Normal;

struct PointLight {
    vec3 position;
    vec3 color;
    vec3 ambientStrength;
    vec3 diffuseStrength;
    vec3 specularStrength;
    float constant;
    float linear;
    float quadratic;
    float shininess;
    bool enabled;
};
struct DirLight {
    vec3 direction;
    vec3 color;
    vec3 ambientStrength;
    vec3 diffuseStrength;
    vec3 specularStrength;
    float constant;
    float linear;
    float quadratic;
    float shininess;
    bool enabled;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    vec3 ambientStrength;
    vec3 diffuseStrength;
    vec3 specularStrength;
    float constant;
    float linear;
    float quadratic;
    float shininess;
    float outcutOff;
    float cutOff;
    bool enabled;
};

#define NR_POINT_LIGHTS 64
#define NR_DIR_LIGHTS 4
#define NR_SPOT_LIGHTS 16
uniform PointLight light[NR_POINT_LIGHTS];
uniform DirLight dirLight[NR_DIR_LIGHTS];
uniform SpotLight spotLight[NR_SPOT_LIGHTS];

// TODO UBO
//layout (std140, binding = 0) uniform Lights {
//    uniform PointLight light[NR_POINT_LIGHTS];
//    uniform DirLight dirLight[NR_DIR_LIGHTS];
//    uniform SpotLight spotLight[NR_SPOT_LIGHTS];
//};
uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

float calculateDiffuse(vec3 modelNormal, vec3 lightDir) {
    return max(dot(modelNormal, lightDir), 0.0f);
}

float calculateSpecular(vec3 viewDir, vec3 reflectDir, float shininess) {
    return pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
}

void main() {
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 modelNormal = texture(gNormal, TexCoords).rgb;
    vec3 modelDiffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float modelSpecular = texture(gAlbedoSpec, TexCoords).a;
    vec3 result = vec3(0.0f);
    for (uint i = 0; i < NR_POINT_LIGHTS; i++) {
        if (light[i].enabled) {
            vec3 lightDir = normalize(light[i].position - FragPos);
            vec3 reflectDir = reflect(-lightDir, modelNormal);
            vec3 ambient = light[i].ambientStrength * light[i].color * modelDiffuse;
            vec3 diffuse = light[i].diffuseStrength * calculateDiffuse(modelNormal, lightDir) * light[i].color * modelDiffuse;
            vec3 specular = light[i].specularStrength * calculateSpecular(viewDir, reflectDir, light[i].shininess) * light[i].color * modelSpecular;
            float distance = length(light[i].position - FragPos);
            if (distance <= 15.0f) {
                result += (ambient + diffuse + specular) / (light[i].constant + light[i].linear * distance + light[i].quadratic * distance * distance);
            }
        }
    }
    for (uint i = 0; i < NR_DIR_LIGHTS; i++) {
        if (dirLight[i].enabled) {
            vec3 lightDir = normalize(dirLight[i].direction);
            vec3 reflectDir = reflect(lightDir, modelNormal);
            vec3 ambient = dirLight[i].ambientStrength * dirLight[i].color * modelDiffuse;
            vec3 diffuse = dirLight[i].diffuseStrength * calculateDiffuse(modelDiffuse, lightDir) * dirLight[i].color * modelDiffuse;
            vec3 specular = dirLight[i].specularStrength * calculateSpecular(lightDir, reflectDir, dirLight[i].shininess) * dirLight[i].color * modelSpecular;
            float distance = length(dirLight[i].direction);
            result += (ambient + diffuse + specular) / (dirLight[i].constant + dirLight[i].linear * distance + dirLight[i].quadratic * distance * distance);
        }
    }
    for (uint i = 0; i < NR_SPOT_LIGHTS; i++) {
        if (spotLight[i].enabled) {
            vec3 lightDir = normalize(spotLight[i].position - FragPos);
            float theta = dot(lightDir, normalize(-spotLight[i].direction));
            float epsilon = spotLight[i].cutOff - spotLight[i].outcutOff;
            float intensity = clamp((theta - spotLight[i].outcutOff) / epsilon, 0.0f, 1.0f);
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, modelNormal);
            vec3 ambient = intensity * spotLight[i].ambientStrength * spotLight[i].color * modelDiffuse;
            vec3 diffuse = intensity * spotLight[i].diffuseStrength * calculateDiffuse(modelDiffuse, lightDir) * spotLight[i].color * modelDiffuse;
            vec3 specular = intensity * spotLight[i].specularStrength * calculateSpecular(viewDir, reflectDir, spotLight[i].shininess) * spotLight[i].color * modelSpecular;
            float distance = length(spotLight[i].position - FragPos);
            if (distance <= 50.0f) {
                result += (ambient + diffuse + specular) / (spotLight[i].constant + spotLight[i].linear * distance + spotLight[i].quadratic * distance * distance);
            }
        }
    }
    result = vec3(1.0f) - exp(-result);
    result = pow(result, vec3(2.2f));
    FragColor = vec4(result, 1.0);

}
