#version 450
in vec2 TexCoord;
out vec4 outColour;

uniform sampler2D font;
uniform vec3 colour;

void main() 
{
	vec3 textColour = colour;
	outColour = vec4(textColour, texture(font, TexCoord).r);
	//outColour = vec4(1.f,0.f,1.f,1.f);
}