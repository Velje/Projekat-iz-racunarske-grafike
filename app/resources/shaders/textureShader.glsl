//#shader vertex
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec2 TexCoords;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}

//#shader fragment
#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_regular1;

void main() {
    FragColor = texture(texture_regular1, TexCoords);
}
