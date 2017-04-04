#version 450
in vec2 position;
in vec2 texcoord;
out vec2 TexCoord;
out vec2 v_blurTexCoords[14];
uniform float pixSize;
uniform float blurRadius = 14.f;
void main()
{
	TexCoord = texcoord;

	float rad = blurRadius * 7.f/7.f * pixSize;
	v_blurTexCoords[0] = TexCoord + vec2(0.0, -rad);
	v_blurTexCoords[13] = TexCoord + vec2(0.0, rad);

	rad = blurRadius * 6.f/7.f * pixSize;
	v_blurTexCoords[1] = TexCoord + vec2(0.0, -rad);
	v_blurTexCoords[12] = TexCoord + vec2(0.0, rad);

	rad = blurRadius * 5.f/7.f * pixSize;
	v_blurTexCoords[2] = TexCoord + vec2(0.0, -rad);
	v_blurTexCoords[11] = TexCoord + vec2(0.0, rad);

	rad = blurRadius * 4.f/7.f * pixSize;
	v_blurTexCoords[3] = TexCoord + vec2(0.0, -rad);
	v_blurTexCoords[10] = TexCoord + vec2(0.0, rad);

	rad = blurRadius * 3.f/7.f * pixSize;
	v_blurTexCoords[4] = TexCoord + vec2(0.0, -rad);
	v_blurTexCoords[9] = TexCoord + vec2(0.0, rad);

	rad = blurRadius * 2.f/7.f * pixSize;
	v_blurTexCoords[5] = TexCoord + vec2(0.0, -rad);
	v_blurTexCoords[8] = TexCoord + vec2(0.0, rad);

	rad = blurRadius * 1.f/7.f * pixSize;
	v_blurTexCoords[6] = TexCoord + vec2(0.0, -rad);
	v_blurTexCoords[7] = TexCoord + vec2(0.0, rad);
	
	gl_Position = vec4(position, 0.0, 1.0);
};