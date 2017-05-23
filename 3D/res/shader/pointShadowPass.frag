#version 450

in vec4 FragPos;
uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	float lightDist = length(FragPos.xyz - lightPos);
	lightDist /= farPlane;
	gl_FragDepth = lightDist;
}