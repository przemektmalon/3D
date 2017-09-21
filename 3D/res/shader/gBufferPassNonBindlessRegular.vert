#version 450

#extension GL_ARB_shader_draw_parameters : require


uniform layout(location = 1) mat4 proj;
//uniform layout(location = 2) mat4 model;
uniform layout(location = 3) mat4 view;
uniform layout(location = 4) vec3 camPos;

layout (location = 0) in vec3 p;
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 t;

layout (binding=4) buffer Pos
{
	mat4 transforms[];
};

out vec2 TexCoord;
out vec3 Normal;
out vec3 ViewVec;
out vec3 WorldPos;
flat out uint DrawID;

out float logz;

#define C 1.f

#define FAR 1000000.f

void main()
{
	const float FC = 2.0 / log2(FAR + 1.0);

	mat4 transform = transforms[gl_DrawIDARB];

	vec4 worldPos = transform * vec4(p, 1.f);
	//vec4 worldPos = vec4(p, 1.f);

	vec4 projPos = proj * view * worldPos;
	gl_Position = projPos;

    logz = 1.0 + gl_Position.w;
    
    //gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * FC - 1.0;

	ViewVec = camPos - worldPos.xyz;

	DrawID = gl_DrawIDARB;

	TexCoord = t;
	//TexCoord = vec2(1.f,1.f);
	WorldPos = worldPos.xyz;
	//Normal = n;
	Normal = transpose(inverse(mat3(transform))) * n;
	//Normal = vec3(1.f,1.f,1.f);
};