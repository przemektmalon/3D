#version 450

in vec2 TexCoord;
//in vec3 ViewRay;
out layout(location = 0) vec4 outColour;

uniform sampler2D tex;
uniform vec4 colour;

//uniform mat4 proj;
//uniform vec3 camPos;

void main() {

    //float depth = texelFetch(gDepth, ivec2(gl_FragCoord.xy), 0).r;
    //float zview = -(proj[3][2])/((2.f*depth-1.f)+proj[2][2]);

    //vec3 vs = ViewRay * zview;

    //outColour = colour + vec4(texture(tex,TexCoord).rgba);
    outColour = colour + vec4(texture(tex,TexCoord).rgba);

    //outColour = colour;
    //outColour = vec4(0.f,0.f,1.f,0.5f);
    //outColour = vec4(vec2(gl_FragCoord.xy)/vec2(1280.f,720.f),0.f,1.f);
    //outColour = vec4(1.f,0.f,1.f,1.f);
};