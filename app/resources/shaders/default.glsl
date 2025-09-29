//#shader vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 instanceMatrix;

out vec2 TexCoords;
out mat3 TBN;
out vec3 FragPos;

layout (std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;
uniform mat3 normalModelMatrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 T = normalize(normalModelMatrix * aTangent);
    vec3 B = normalize(normalModelMatrix * aBitangent);
    vec3 N = normalize(normalModelMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    B = normalize(cross(N, T));
    N = normalize(N);
    TBN = mat3(T, B, N);
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
in mat3 TBN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

void main() {
    gPosition.xyz = FragPos.xyz;
    vec3 normal = texture(texture_normal1, TexCoords).rgb * 2.0f - 1.0f;
    gNormal.rgb = TBN * normal * 0.5f + 0.5f;
    if (gl_FrontFacing) {
        gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
        gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
    }
    else {
        gAlbedoSpec.rgb = vec3(0.0f);
        gAlbedoSpec.a = 0.0f;
    }
}

