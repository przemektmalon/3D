#version 450
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout (binding=2) buffer CameraMatrix
{
    mat4 cameraMatrix[6];
};

out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = cameraMatrix[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  