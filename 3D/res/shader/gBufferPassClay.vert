#version 450

#extension GL_ARB_shader_draw_parameters : require

uniform layout(location = 1) mat4 proj;
uniform layout(location = 3) mat4 view;
uniform layout(location = 4) vec3 camPos;

layout (location = 0) in vec3 p;
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 t;

layout (binding=4) buffer Pos
{
	mat4 transforms[];
};

out vec3 Normal;
out vec3 ViewVec;
out vec3 WorldPos;
flat out uint DrawID;

out float logz;

void main()
{
	mat4 transform = transforms[gl_DrawIDARB];

	vec4 worldPos = transform * vec4(p, 1.f);

	vec4 projPos = proj * view * worldPos;
	gl_Position = projPos;

    logz = 1.0 + gl_Position.w;

	ViewVec = camPos - worldPos.xyz;

	DrawID = gl_DrawIDARB;

	WorldPos = worldPos.xyz;
	Normal = transpose(inverse(mat3(transform))) * n;
};