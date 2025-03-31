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
};

uniform PointLight light;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

float calculateDiffuse(vec3 modelNormal, vec3 lightDir);
float calculateSpecular(vec3 viewDir, vec3 reflectDir);

void main() {
    vec3 modelNormal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, modelNormal);
    vec3 ambient = light.ambientStrength * light.color;
    vec3 modelDiffuse = texture(texture_diffuse1, TexCoords).rgb;
    float modelSpecular = texture(texture_specular1, TexCoords).r;
    vec3 diffuse = light.diffuseStrength * calculateDiffuse(modelNormal, lightDir) * light.color * modelDiffuse;
    vec3 specular = light.specularStrength * calculateSpecular(viewDir, reflectDir) * light.color * modelSpecular;
    float distance = length(light.position - FragPos);
    vec3 finalLight = min((ambient + diffuse + specular) / (light.constant + light.linear * distance + light.quadratic * distance * distance), vec3(1.0f));
    FragColor = vec4(finalLight, 1.0);
}

float calculateDiffuse(vec3 modelNormal, vec3 lightDir) {
    return max(dot(modelNormal, lightDir), 0.0f);
}

float calculateSpecular(vec3 viewDir, vec3 reflectDir) {
    return pow(max(dot(viewDir, reflectDir), 0.0f), light.shininess);
}

