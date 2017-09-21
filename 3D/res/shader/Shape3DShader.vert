#version 450

uniform layout(location = 1) mat4 pvm;

layout (location = 0) in vec3 p;
layout (location = 1) in vec2 t;

out vec2 TexCoord;

void main()
{
    gl_Position = pvm * vec4(p, 1.0f);
    TexCoord = t;
}