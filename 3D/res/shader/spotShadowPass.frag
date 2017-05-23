#version 450

in float logz;

layout (location = 0) out vec4 outColour;

#define FAR 1000000.f

void main()
{
	//const float FCHalf = 1.0 / log2(FAR + 1.0);
	//gl_FragDepth = log2(logz) * FCHalf;
	//gl_FragDepth = 0.5;

	//outColour = vec4(vec3(gl_FragDepth),1.f);
}