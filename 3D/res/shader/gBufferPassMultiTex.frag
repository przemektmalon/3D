#version 450
#extension GL_NV_gpu_shader5 : require
#extension GL_ARB_bindless_texture : require
//#extension GL_ARB_shader_draw_parameters : require

layout (location = 0) out f16vec2 gNormal;
//layout (location = 0) out vec3 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;
layout (location = 2) out uint gID;

in vec2 TexCoord;
in vec3 Normal;
in vec3 ViewVec;
in vec3 WorldPos;
flat in uint DrawID;

layout(binding = 0) uniform sampler2D diffuse;
layout(binding = 1) uniform sampler2D specular;
layout(binding = 2) uniform sampler2D normal;

layout(bindless_sampler) uniform sampler2D textureID;

layout(std430, binding=3) buffer TextureHandleBuffer
{
    uvec4 texHandle[];
};

layout(std140, binding=6) buffer InstanceIDBuffer
{
    uint id[];
};

//layout(binding = 3) uniform sampler2D bump;
//uniform layout(location = 4) vec3 camPos;
//uniform uint id;

#define FAR 1000000.f
#define C 1.f

in float logz;

uvec4 texHandlesA;
uvec4 texHandlesB;
uvec4 texHandlesC;
uvec4 texHandlesD;
uvec4 texHandlesE;
uvec4 texHandlesF;
uvec4 texHandlesG;
uint numBlends;
vec3 alpha;


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

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord)
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    //vec3 map = texture2D(sampler2D(normalTexHandle, texcoord)).xyz * 2.f - 1.f;

    float weightBack = 1 - (alpha.r + alpha.g + alpha.b);

    vec3 map = (texture2D(sampler2D(texHandlesB.rg), texcoord).xyz * 2.f - 1.f) * weightBack;
    map += (texture2D(sampler2D(texHandlesC.ba), texcoord).xyz * 2.f - 1.f) * alpha.r;
    map += (texture2D(sampler2D(texHandlesC.ba), texcoord).xyz * 2.f - 1.f) * alpha.g;
    map += (texture2D(sampler2D(texHandlesC.ba), texcoord).xyz * 2.f - 1.f) * alpha.b;

    //map = map * 255.f/127.f - 128.f/127.f;

    //map.y = -map.y;



    mat3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( TBN * map );
    //return N;
}

/*vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(bump, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * 0.0001f);
    return texCoords - p;
}*/



void main()
{
	//const float FCHalf = (1.0/log(FAR*C + 1)) * 0.5;
	const float FCHalf = 1.0 / log2(FAR + 1.0);
	
	vec2 texCoord = TexCoord;

	vec3 viewVec = ViewVec;

	gl_FragDepth = log2(logz) * FCHalf;

    texHandlesA = texHandle[7*DrawID + 0]; //rg == albedoA, ba == specA
    texHandlesB = texHandle[7*DrawID + 1]; //rg == normalA, ba == albedoB
    texHandlesC = texHandle[7*DrawID + 2]; //rg == specB, ba == normalB
    texHandlesD = texHandle[7*DrawID + 3]; //rg == albedoC, ba == specC
    texHandlesE = texHandle[7*DrawID + 4]; //rg == normalC, ba == albedoD
    texHandlesF = texHandle[7*DrawID + 5]; //rg == specD, ba == normalD
    texHandlesG = texHandle[7*DrawID + 6]; //rg == alpha, ba == UNUSED

    alpha = texture(sampler2D(texHandlesG.rg), TexCoord).rgb;

    //numBlends = int(texHandlesA.r != 0) + int(texHandlesB.b != 0) + int(texHandlesD.r != 0) + int(texHandlesE.r != 0);

    vec3 mappedNormal = perturb_normal(normalize(Normal), normalize(viewVec), texCoord);
    mappedNormal = Normal;

    gNormal = encode(normalize(mappedNormal));

    float weightBack = 1 - (alpha.r + alpha.g + alpha.b);

    vec4 albedoSpec;
    albedoSpec.rgb = texture(sampler2D(texHandlesA.rg), TexCoord).rgb * weightBack;
    albedoSpec.rgb += texture(sampler2D(texHandlesB.ba), TexCoord).rgb * alpha.r;
    albedoSpec.rgb += texture(sampler2D(texHandlesD.rg), TexCoord).rgb * alpha.g;
    albedoSpec.rgb += texture(sampler2D(texHandlesE.ba), TexCoord).rgb * alpha.b;

    albedoSpec.a = texture(sampler2D(texHandlesA.ba), TexCoord).r * weightBack;
    albedoSpec.a += texture(sampler2D(texHandlesC.rg), TexCoord).r * alpha.r;
    albedoSpec.a += texture(sampler2D(texHandlesD.ba), TexCoord).r * alpha.g;
    albedoSpec.a += texture(sampler2D(texHandlesF.rg), TexCoord).r * alpha.b;
    
    //albedoSpec.rgba = vec4(0.f,1.f,1.f,1.f);

    gAlbedoSpec = albedoSpec;

    gID = id[DrawID];
}