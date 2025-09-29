//#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
out vec3 TexCoords;

layout (std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main() {
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 0.0);
    gl_Position = pos.xyww;
}

//#shader fragment
#version 460 core

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube skybox;

void main() {
    FragColor = texture(skybox, TexCoords);
}
