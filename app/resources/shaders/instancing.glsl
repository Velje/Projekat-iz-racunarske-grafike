//#shader vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 instanceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

layout (std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform mat3 normalModelMatrix;
uniform mat4 transformedModel;

void main() {
    mat4 transformedInstance = transformedModel;
    transformedInstance[3] += instanceMatrix[3];
    vs_out.FragPos = vec3(transformedInstance * vec4(aPos, 1.0));
    vec3 T = normalize(normalModelMatrix * aTangent);
    vec3 B = normalize(normalModelMatrix * aBitangent);
    vec3 N = normalize(normalModelMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    B = normalize(cross(N, T));
    N = normalize(N);
    vs_out.TBN = mat3(T, B, N);
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}

//#shader fragment
#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

void main() {
    gPosition.xyz = fs_in.FragPos.xyz;
    vec3 mappedNormal = normalize(fs_in.TBN * (texture(texture_normal1, fs_in.TexCoords).xyz * 2.0f - 1.0f));
    gNormal.rgb = mappedNormal * 0.5f + 0.5f;
    gAlbedoSpec.rgb = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    gAlbedoSpec.a = texture(texture_specular1, fs_in.TexCoords).r;
}
