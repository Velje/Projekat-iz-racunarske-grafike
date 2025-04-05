//#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalModelMatrix;
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalModelMatrix * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoords = aTexCoords;
}

//#shader fragment
#version 460 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

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

#define NR_DIR_LIGHTS 2

uniform DirLight dirLight[NR_DIR_LIGHTS];
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

float calculateDiffuse(vec3 modelNormal, vec3 lightDir) {
    return max(dot(modelNormal, lightDir), 0.0f);
}

float calculateSpecular(vec3 viewDir, vec3 reflectDir, float shininess) {
    return pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
}

void main() {
    vec3 meshNormal = normalize(Normal);
    vec3 floorDiffuse = texture(texture_diffuse1, TexCoords).rgb;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0f);
    for (uint i = 0; i < 2; i++) {
        if (dirLight[i].enabled) {
            vec3 lightDir = normalize(dirLight[i].direction);
            vec3 reflectDir = reflect(lightDir, meshNormal);
            vec3 ambient = dirLight[i].ambientStrength * dirLight[i].color * floorDiffuse;
            vec3 diffuse = dirLight[i].diffuseStrength * calculateDiffuse(meshNormal, -lightDir) * dirLight[i].color * floorDiffuse;
            vec3 specular = dirLight[i].specularStrength * calculateSpecular(viewDir, reflectDir, dirLight[i].shininess) * dirLight[i].color;
            float distance = length(FragPos - dirLight[i].direction);
            result += (ambient + diffuse + specular) / (dirLight[i].constant + dirLight[i].linear * distance + dirLight[i].quadratic * distance * distance);
        }
    }
    result = min(result, vec3(1.0f));
    FragColor = vec4(result, 1.0f);
}
