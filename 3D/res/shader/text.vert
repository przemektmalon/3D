#version 450
in vec3 position;
in vec2 texCoord;
out vec2 TexCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main() 
{
	TexCoord = texCoord;
	gl_Position = proj * view * vec4(position, 1.0);
}