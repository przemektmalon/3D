#version 450
in vec3 TexCoord;

layout (location = 0) out vec3 outColour;

uniform samplerCube skybox;

uniform layout(location = 1) float exposure = 1.f;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
	return vec3(((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F);
}

void main()
{
    outColour = vec3(texture(skybox, TexCoord));

	outColour *= vec3(1.f);
	vec3 curr = Uncharted2Tonemap(exposure*outColour);

	vec3 whiteScale = vec3(1.f)/Uncharted2Tonemap(vec3(W));
	vec3 colour = curr*whiteScale;
	outColour = pow(colour,vec3(1/2.2));

    gl_FragDepth = 10000.f;
}