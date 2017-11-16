#version 450
out layout(location = 0) vec4 outColour;

#define FAR 1000000.f

in float logz;

uniform vec4 wireColour;

void main() 
{
	const float FCHalf = 1.0 / log2(FAR + 1.0);
	gl_FragDepth = log2(logz) * FCHalf;

	outColour = wireColour;
};