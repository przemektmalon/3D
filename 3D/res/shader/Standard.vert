#version 450

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec2 viewRay;

out vec2 TexCoord;
out vec3 ViewRay;

uniform mat4 view;

void main()
{
    gl_Position = vec4(position, 0.f, 1.0f);
    TexCoord = texCoord;
    ViewRay = vec3(vec4(viewRay,1.f,1.f) * view);
}