#version 450
#extension GL_NV_gpu_shader5 : require
//#extension GL_ARB_bindless_texture : require
//#extension GL_ARB_shader_draw_parameters : require

in vec2 TexCoord;
in vec3 Normal;
in vec3 ViewVec;
in vec3 WorldPos;
flat in uint DrawID;

f16vec2 encode (vec3 n)
{
    float16_t p = sqrt((float16_t)n.z*(float16_t)8.f+(float16_t)8.f);
    return f16vec2((f16vec2)n.xy/p + (float16_t)0.5f);
}

mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( normalize(T * invmax), normalize(B * invmax), N );
}



layout (location = 0) out f16vec2 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;
layout (location = 2) out uint gID;

uniform sampler2DArray textureArray;
uniform sampler2D tex;

//For each instance, offset into texture array, to access correct texture
layout(std430, binding=9) buffer TextureArrayIndices
{
    ivec2 texIndex[]; //4 shorts: albedo, normal, specular, 4th unused
};

layout(std140, binding=6) buffer InstanceIDBuffer
{
    uint id[];
};

#define FAR 1000000.f
#define C 1.f

in float logz;

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord)
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    //vec3 map = texture2D(sampler2D(texHandle[DrawID].zw), texcoord).xyz * 2.f - 1.f;
    int layer = texIndex[DrawID].g;
    vec3 map = texture(textureArray, vec3(TexCoord, 1)).rgb * 2.f - 1.f;
    //vec3 map = vec3(0.f);
    //map = map * 255.f/127.f - 128.f/127.f;

    //map.y = -map.y;



    mat3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( TBN * map );
    //return N;
}

void main()
{
	const float FCHalf = 1.0 / log2(FAR + 1.0);
	
	vec2 texCoord = TexCoord;
	vec3 viewVec = ViewVec;
	gl_FragDepth = log2(logz) * FCHalf;
    vec3 mappedNormal = perturb_normal(normalize(Normal), normalize(viewVec), texCoord);
    gNormal = encode(normalize(mappedNormal));
    
    //int layer = texIndex[DrawID].r;
    int layer = 0;
    //gAlbedoSpec.rgb = texture(tex, vec2(texCoord)).rgb;
    gAlbedoSpec.rgb = texture(textureArray, vec3(texCoord, layer)).rgb;
    //gAlbedoSpec.rgb = vec3(texCoord,1.f);
    //gAlbedoSpec.rgb = texture(sampler2D(texHandle[DrawID].xy), TexCoord).rgb;

    //gAlbedoSpec.rgb = vec3(1.f,0.f,1.f);

    gAlbedoSpec.a = 0.01f;

    //gAlbedoSpec.a = texture(sampler2D(texHandle[DrawID].xy), texCoord).r;


    gID = id[DrawID];
}