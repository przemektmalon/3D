#version 450
layout (location = 0) out vec4 outColour;
in vec2 TexCoord;
uniform vec2 noiseScale;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform sampler2D gDepth;

uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
const int kernelSize = 64;
const float radius = 20.f;

// tile noise texture over screen based on screen dimensions divided by noise size
//const vec2 noiseScale = vec2(1920.0f/4.0f, 1080.0f/4.0f); 

uniform mat4 proj;
uniform mat4 view;

const float NEAR = 5.f; // projection matrix's near plane
const float FAR = 10000.0f; // projection matrix's far plane
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));	
}

uniform float ssaoPower = 1.f;



void main()
{
    vec3 worldSpace = texture(gPosition, TexCoord).xyz;
    vec3 origin = vec3(view * vec4(worldSpace,1.f));

    //origin.z = texelFetch(gDepth, ivec2(gl_FragCoord.xy) , 0).r;

    //outColour = vec4(vec3(1.f - origin.z) * 100.f,1.f);
    //return;

    if(origin.r != 0.f)
    {
        //origin.x += view[3][0];
        //origin.y += view[3][1];
        //origin.z += view[3][2];
    }


    vec3 normal = texture(gNormal, TexCoord).rgb;
    
    if(length(normal) <= 0.0001)
    {
        //outColour = 1.f; return;
    }

    normal = (vec3(transpose(inverse(view)) * vec4(normal,1.f)));
    normal = normalize(normal);

    vec3 normal2 = normalize(cross(dFdy(origin), dFdx(origin)));
    normal = normal2;

    //normal2 = normal2 * 0.5f + 0.5f;
    //outColour = vec4(normal2,1.f);
    outColour = vec4(vec3(LinearizeDepth(origin.z)),1.f);
    return;

    vec3 rvec = texture(texNoise, TexCoord * noiseScale).xyz * 2.0 - 1.0;
    
    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);



    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i) 
    {
        vec3 ssample = tbn * samples[i];
        ssample = (ssample * radius) + origin;
        
        vec4 offset = vec4(ssample, 1.0);
        offset = proj * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;

        vec3 worldSpace = texture(gPosition, offset.xy).xyz;
        vec3 ofsample = vec3(view * vec4(worldSpace,1.f));

        float sampleDepth = ofsample.z;
        //float sampleDepth = texture(gDepth, offset.xy).r;

        //outColour = vec4(vec3(offset.xy,0.f),1.f);return;

        if(length(texture(gNormal,offset.xy).xyz) >= 0.01)
        {
            float rangeCheck = origin.z - sampleDepth < radius ? 1.0 : 0.0;
            occlusion += (sampleDepth <= ssample.z ? 1.0 : 0.0) * rangeCheck;
        }
        else
        {
            occlusion += 1.0;
        }
    }
    occlusion = (occlusion / kernelSize) + 0.05f;//SSAO 'SHARPNESS'
    //if(occlusion == 0.05) { occlusion = 1.f; }
    occlusion = pow(occlusion, ssaoPower);
    outColour = vec4(vec3(occlusion),1.f);
}