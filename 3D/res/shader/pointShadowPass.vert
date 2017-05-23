#version 450

#extension GL_ARB_shader_draw_parameters : require

layout (location = 0) in vec3 p;

//uniform layout(location = 2) mat4 tr;

/*layout (binding=0) buffer TransformIndex
{
	int transformID[];
};*/

layout (binding=1) buffer Pos
{
	mat4 transforms[];
};

void main()
{
	//mat4 transform = transforms[transformID[gl_DrawIDARB]];
	mat4 transform = transforms[gl_DrawIDARB];
	gl_Position = transform * vec4(p, 1.f);
};