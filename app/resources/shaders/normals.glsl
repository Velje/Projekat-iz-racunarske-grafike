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
    vec3 Normal;
} vs_out;

layout (std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;
uniform mat3 normalModelMatrix;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
    vec3 T = normalize(normalModelMatrix * aTangent);
    vec3 B = normalize(normalModelMatrix * aBitangent);
    vec3 N = normalize(normalModelMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    B = normalize(cross(N, T));
    N = normalize(N);
    vs_out.Normal = N;
    vs_out.TBN = mat3(T, B, N);
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = normalize(normalModelMatrix * aNormal);
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0f);
}

//#shader geometry
#version 460 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout (std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
    vec3 Normal;
} gs_in[];

out GS_OUT {
    vec3 FragPos;
    vec3 Normal;
} gs_out;

uniform sampler2D texture_normal1;

void main() {
    for (uint i = 0; i < 3; ++i) {
        vec3 mappedNormal = normalize(gs_in[i].TBN * (texture(texture_normal1, gs_in[i].TexCoords).rgb * 2.0f - 1.0f));
        vec3 start = gs_in[i].FragPos;
        gs_out.FragPos = start;
        gs_out.Normal = mappedNormal;
        vec3 end = start + mappedNormal;

        gl_Position = projection * view * vec4(start, 1.0f);
        EmitVertex();

        gl_Position = projection * view * vec4(end, 1.0f);
        EmitVertex();
    }
    EndPrimitive();

}

//#shader fragment
#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in GS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

void main() {
    gPosition.xyz = fs_in.FragPos.xyz;
    gNormal.rgb = fs_in.Normal.rgb * 0.5f + 0.5f;
    gAlbedoSpec.rgba = vec4(1.0f, 1.0f, 1.0f, 2048.0f);
}
