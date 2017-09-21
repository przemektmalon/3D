#version 450

in vec2 TexCoord;
out layout(location = 0) vec4 outColour;

uniform sampler2D tex;

void main() {
    outColour = vec4(texture(tex,TexCoord).rgba);
    //outColour = vec4(1.f,0.f,1.f,1.f);
};