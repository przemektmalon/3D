#version 450
layout (location = 0) in vec3 position;
out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 view;
out vec3 FragPos;

void main()
{
    vec4 pos = projection * view * vec4(position, 1.f);
    //pos.z = 1.f;
    //pos.w = 1.f;
    gl_Position = pos;
    TexCoord = position;
};