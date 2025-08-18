//#shader vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalModelMatrix;

void main() {
    FragPos = vec3(instanceMatrix * vec4(aPos, 1.0));
    Normal = normalModelMatrix * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

out vec4 FragColor;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

void main() {
    gPosition = FragPos.xyz;
    gNormal = normalize(texture(texture_normal1, TexCoords).rgb * 2.0f - 1.0f + Normal);
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}