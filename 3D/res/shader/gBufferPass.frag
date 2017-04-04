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
    vec3 map = texture2D(sampler2D(texHandle[DrawID].zw), texcoord).xyz * 2.f - 1.f;

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

	//mat3 TBN = transpose(cotangent_frame(normalize(Normal), normalize(-viewVec), texCoord));
	//vec3 viewDir = normalize((TBN * WorldPos) - (TBN * camPos));


	//texCoord = ParallaxMapping(vec2(vec3(texCoord,1.f)), viewVec);

	//texCoord = ParallaxMapping(texCoord, normalize((cotangent_frame(Normal, -normalize(viewVec), texCoord)) * (-viewVec)));
	//texCoord = ParallaxMapping(texCoord, normalize(-viewVec));
	//if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
    	//discard;

	//vec4 clipPos = gl_FragCoord;
	//float depth = (2.f*log(C*clipPos.w + 1.f) / log(C*Far + 1.f) - 1.f) * clipPos.w;
	//gl_FragDepth = depth;
	//gl_FragDepth = log(logz)*FC; 
	//gl_FragDepth = logz;
	gl_FragDepth = log2(logz) * FCHalf;

    vec3 mappedNormal = perturb_normal(normalize(Normal), normalize(viewVec), texCoord);

    //mappedNormal = texture(normal, TexCoord).xyz;

    //mappedNormal = mappedNormal * 255.f/127.f - 128.f/127.f;

    //mappedNormal.y = -mappedNormal.y;

    //gNormal = normalize(Normal);
    //gNormal = vec3(1.f,0.f,1.f)
    

    gNormal = encode(normalize(mappedNormal));
    

    //gNormal = f16vec2(vec2(0.2f,0.5f));
    //gAlbedoSpec.rgb = texture(diffuse, texCoord).rgb;
    
    gAlbedoSpec.rgb = texture(sampler2D(texHandle[DrawID].xy), TexCoord).rgb;
    //gAlbedoSpec.rgb = vec3(1.f,0.f,1.f);

    //gAlbedoSpec.rgb = normalize(viewVec);
    //gAlbedoSpec.rgb = vec3(texCoord,1.f);
    

    gAlbedoSpec.a = texture(sampler2D(texHandle[DrawID].xy), texCoord).r;
    

    //gAlbedoSpec.a = texture(specular, texCoord).r;
    //gAlbedoSpec.a = 1.f;
    

    gID = id[DrawID];


    //gl_FragColor = vec4(vec3(WorldPos),1.f);
}