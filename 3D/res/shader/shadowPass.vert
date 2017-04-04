#version 450
uniform layout(location = 1) mat4 proj;
uniform layout(location = 2) mat4 model;
uniform layout(location = 3) mat4 view;

layout (location = 0) in vec3 p;

//out float ViewSpaceDepth;

void main()
{
	vec4 viewSpace = view * model * vec4(p,1.f);
	//ViewSpaceDepth = viewSpace.z;
	gl_Position = proj * viewSpace;
};