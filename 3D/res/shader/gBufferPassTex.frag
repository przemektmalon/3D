#version 450
#extension GL_NV_gpu_shader5 : require
#extension GL_ARB_bindless_texture : require

layout (location = 0) out f16vec2 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;
layout (location = 2) out vec4 gPBR;

in vec2 TexCoord;
in vec3 Normal;
in vec3 ViewVec;
in vec3 WorldPos;
flat in uint DrawID;

layout(std430, binding=3) buffer TextureHandleBuffer
{
    uvec2 texHandle[];
};

#define FAR 1000000.f

in float logz;

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
    vec3 map = texture2D(sampler2D(texHandle[6*DrawID+1]), texcoord).xyz * 2.f - 1.f;
    mat3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( TBN * map );
}

void main()
{
    const float FCHalf = 1.0 / log2(FAR + 1.0);
    
    vec2 texCoord = TexCoord;

    vec3 viewVec = ViewVec;

    gl_FragDepth = log2(logz) * FCHalf;

    vec3 mappedNormal = perturb_normal(normalize(Normal), normalize(viewVec), texCoord);

    gNormal = encode(normalize(mappedNormal));

    gAlbedoSpec.rgb = texture(sampler2D(texHandle[6*DrawID]), TexCoord).rgb;

    gAlbedoSpec.a = texture(sampler2D(texHandle[6*DrawID+2]), texCoord).r;

    if (texHandle[6*DrawID+3] != 0)
    {
        gPBR.rgba = vec4(texture(sampler2D(texHandle[6*DrawID+3]), texCoord).r, 1.f, 0.f, 1.f);
    }
    else
    {
        gPBR.rgba = vec4(0.f,0.f,0.f,1.f);
    }
}