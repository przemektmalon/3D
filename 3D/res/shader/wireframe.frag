#version 450
out layout(location = 0) vec4 outColour;

#define FAR 1000000.f

in float logz;

void main() 
{
	const float FCHalf = 1.0 / log2(FAR + 1.0);
	gl_FragDepth = log2(logz) * FCHalf;

	outColour = vec4(1.f,1.f,1.f,1.f);
};