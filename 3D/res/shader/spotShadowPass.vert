#version 450

#extension GL_ARB_shader_draw_parameters : require

layout (location = 0) in vec3 p;

uniform layout(location = 2) mat4 projView;

/*layout (binding=0) buffer TransformIndex
{
	int transformID[];
};*/

layout (binding=4) buffer Pos
{
	mat4 transforms[];
};

#define FAR 1000000.f

out float logz;

void main()
{
	const float FC = 2.0 / log2(FAR + 1.0);

	//mat4 transform = mat4(1.f);
	mat4 transform = transforms[gl_DrawIDARB];
	gl_Position = projView * transform * vec4(p, 1.f);

	//logz = 1.0 + gl_Position.w;

	//gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * FC - 1.0;
};