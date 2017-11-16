#version 450

#extension GL_ARB_shader_draw_parameters : require

uniform layout(location = 1) mat4 proj;
uniform layout(location = 2) mat4 view;

layout (location = 0) in vec3 p;

layout (binding = 1) buffer Pos
{
	mat4 transforms[];
};

out float logz;

void main()
{
	mat4 transform = transforms[gl_DrawIDARB];

	vec4 projPos = proj * view * transform * vec4(p, 1.f);
	gl_Position = projPos;

	logz = 1.0 + gl_Position.w;
}