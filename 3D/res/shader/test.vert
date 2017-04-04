#version 450

uniform layout(location = 1) mat4 proj;
uniform layout(location = 2) mat4 model;
uniform layout(location = 3) mat4 view;

layout (location = 0) in vec2 p;
layout (location = 1) in vec2 t;
layout (location = 2) in vec2 v;

out vec2 TexCoord;
//out vec3 ViewRay;

//uniform mat4 view;

/*void main()
{
	vec4 worldPos = model * vec4(p,1.f);
	vec4 clipPos = proj * view * worldPos;
	gl_Position = clipPos;
	TexCoord = t;
	//Normal = transpose(inverse(mat3(model))) * n;
};*/

void main()
{
    gl_Position = vec4(p, 0.f, 1.0f);
    TexCoord = t;
    //ViewRay = vec3(vec4(viewRay,1.f,1.f) * view);
}