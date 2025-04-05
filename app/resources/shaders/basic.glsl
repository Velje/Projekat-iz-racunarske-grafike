//#shader vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalModelMatrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalModelMatrix * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
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

#define NR_POINT_LIGHTS 2
#define NR_DIR_LIGHTS 2

uniform PointLight light[NR_POINT_LIGHTS];
uniform DirLight dirLight[NR_DIR_LIGHTS];

uniform vec3 viewPos;
uniform sampler2D texture_normal1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_ao1;

float calculateDiffuse(vec3 modelNormal, vec3 lightDir) {
    return max(dot(modelNormal, lightDir), 0.0f);
}

float calculateSpecular(vec3 viewDir, vec3 reflectDir, float shininess) {
    return pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
}

void main() {
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 modelNormal = normalize(texture(texture_normal1, TexCoords).rgb * 2.0f - 1.0f + Normal);
    float modelAO = texture(texture_ao1, TexCoords).r;
    vec3 modelDiffuse = texture(texture_diffuse1, TexCoords).rgb * modelAO;
    float modelSpecular = texture(texture_specular1, TexCoords).r;
    vec3 result = vec3(0.0f);
    for (uint i = 0; i < NR_POINT_LIGHTS; i++) {
        if (light[i].enabled) {
            vec3 lightDir = normalize(light[i].position - FragPos);
            vec3 reflectDir = reflect(-lightDir, modelNormal);
            vec3 ambient = light[i].ambientStrength * light[i].color * modelDiffuse;
            vec3 diffuse = light[i].diffuseStrength * calculateDiffuse(modelNormal, lightDir) * light[i].color * modelDiffuse;
            vec3 specular = light[i].specularStrength * calculateSpecular(viewDir, reflectDir, light[i].shininess) * light[i].color * modelSpecular;
            float distance = length(light[i].position - FragPos);
            result += (ambient + diffuse + specular) / (light[i].constant + light[i].linear * distance + light[i].quadratic * distance * distance);
        }
    }
    for (uint i = 0; i < NR_DIR_LIGHTS; i++) {
        if (dirLight[i].enabled) {
            vec3 lightDir = normalize(dirLight[i].direction);
            vec3 reflectDir = reflect(lightDir, modelNormal);
            vec3 ambient = dirLight[i].ambientStrength * dirLight[i].color * modelDiffuse;
            vec3 diffuse = dirLight[i].diffuseStrength * calculateDiffuse(modelDiffuse, lightDir) * dirLight[i].color * modelDiffuse;
            vec3 specular = dirLight[i].specularStrength * calculateSpecular(lightDir, reflectDir, dirLight[i].shininess) * dirLight[i].color * modelSpecular;
            float distance = length(FragPos + dirLight[i].direction);
            result += (ambient + diffuse + specular) / (dirLight[i].constant + dirLight[i].linear * distance + dirLight[i].quadratic * distance * distance);
        }
    }
    result = min(result, vec3(1.0f));
    FragColor = vec4(result, 1.0);

}


