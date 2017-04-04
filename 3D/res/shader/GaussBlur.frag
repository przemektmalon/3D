#version 450
in vec2 TexCoord;
out vec3 outColor;
uniform sampler2D s_texture;
in vec2 v_blurTexCoords[14];
void main()
{
	//outColor = texture(s_texture, v_blurTexCoords[0]).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[0])*0.0044299121055113265).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[1])*0.00895781211794).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[2])*0.0215963866053).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[3])*0.0443683338718).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[4])*0.0776744219933).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[5])*0.115876621105).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[6])*0.147308056121).rgb;
	outColor += (texture(s_texture, TexCoord)*0.159576912161).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[7])*0.147308056121).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[8])*0.115876621105).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[9])*0.0776744219933).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[10])*0.0443683338718).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[11])*0.0215963866053).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[12])*0.00895781211794).rgb;
	outColor += (texture(s_texture, v_blurTexCoords[13])*0.0044299121055113265).rgb;
};