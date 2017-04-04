#version 450

uniform layout(location = 1) mat4 projModel;

layout (location = 0) in vec3 p;
layout (location = 1) in vec2 t;

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
    gl_Position = projModel * vec4(p, 1.0f);
    TexCoord = t;
    //ViewRay = vec3(vec4(viewRay,1.f,1.f) * view);
}