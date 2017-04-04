#version 450
#extension GL_NV_gpu_shader5 : enable

in vec2 TexCoord;
in vec3 ViewRay;
out layout(location = 0) vec4 outColour;

uniform layout(location = 0) vec3 viewPos; //0
uniform mat4 proj;
uniform mat4 view;


uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssaoTex;
uniform sampler2D shadow;

uniform sampler2D gDepthLin;

struct PointLight
{
	vec4 posRad;
	vec4 colQuad;
	vec4 linear;
};

layout(std430, binding=2) buffer myBuffer
{
	PointLight pointLights[];
};

struct SpotLight
{
	vec4 posRad;
	vec4 dirInner;
	vec4 colOuter;
	vec4 linQuad;
};

layout(std430, binding=3) buffer myBuffer2
{
	SpotLight spotLights[];
};

struct DirectLight
{
	vec3 direction;
	vec3 colour;
};

#define NR_POINT_LIGHTS 2
#define NR_SPOT_LIGHTS 1
#define NR_DIRECT_LIGHTS 1

//uniform PointLight pointLights[NR_POINT_LIGHTS];
//uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform DirectLight directLights[NR_DIRECT_LIGHTS];

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
	return vec3(((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F);
}

uniform float exposure = 1.f;
uniform bool filmic = true;
uniform mat4 lightMat;

const float NEAR = 5.f; // projection matrix's near plane
const float FAR = 10000.0f; // projection matrix's far plane
float LinearizeDepth(float depth)
{
    //float z = depth * 2.0 - 1.0; // Back to NDC 
    //return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
    vec3 c = vec3(NEAR * FAR, NEAR - FAR, FAR);
    float r = c.x / ((depth * c.y) + c.z);
    return r;
}

float LinearizeDepth2(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    //float z = depth;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
    //vec3 c = vec3(NEAR * FAR, NEAR - FAR, FAR);
    //float r = c.x / ((depth * c.y) + c.z);
    //return r;
}

f16vec3 decode (f16vec2 enc)
{
    f16vec2 fenc = enc*(float16_t)4.f-(float16_t)2.f;
    float16_t f = dot(fenc,fenc);
    float16_t g = sqrt((float16_t)1.f-f/(float16_t)4.f);
    f16vec3 n;
    n.xy = fenc*g;
    n.z = (float16_t)1.f-f/(float16_t)2.f;
    return n;
}

void main() 
{
	float depth = texelFetch(gDepth, ivec2(gl_FragCoord.xy),0).r;

	float zView = exp2(depth * log2(FAR + 1.0)) - 1.f;

	vec3 fragPos = viewPos + (ViewRay * -zView);
    f16vec3 normal = decode(f16vec2(texture(gNormal, TexCoord).rg));

	vec3 albedo = texture(gAlbedoSpec, TexCoord).rgb;
    float Specular = texture(gAlbedoSpec, TexCoord).a;
    float ssao = texture(ssaoTex, TexCoord).r;

	//Ambient
	vec3 norm = vec3(vec4(normal,1.f));
	vec3 wFragPos = fragPos.xyz;

	vec3 ambient  = 0.2f * albedo * ssao;
	vec3 viewDir = normalize(viewPos - wFragPos.xyz);

	/*for(int i = 0; i < NR_POINT_LIGHTS; ++i)
	{
		//vec3 norm = normalize(normal);
		vec3 lightDir = normalize(pointLights[i].position - wFragPos);  
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse  = pointLights[i].colour  * diff * albedo;  
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfwayDir), 0.0), 16); 
		vec3 specular = pointLights[i].colour * spec * Specular;

		float distance = length(pointLights[i].position - wFragPos);
		float attenuation = 1.f / (1.f + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);
		diffuse *= attenuation;
		specular *= attenuation;
		ambient += (diffuse + specular);
	}*/
	
	/*for(int i = 0; i < NR_POINT_LIGHTS; ++i)
	{
		//if(pos[i].xyz == vec3(110.f,110.f,110.f))
		//vec3 norm = normalize(normal);
		vec3 lightDir = normalize(pointLights[i].posLin.xyz - wFragPos);  
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse  = pointLights[i].colQuad.xyz  * diff * albedo;  
		//vec3 diffuse  = pointLights[i].colQuad.xyz  * diff * albedo;  
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfwayDir), 0.0), 16); 
		vec3 specular = pointLights[i].colQuad.xyz * spec * Specular;
		//vec3 specular = vec3(10.f,10.f,10.f) * spec * Specular;

		float distance = length(pointLights[i].posLin.xyz - wFragPos);
		float attenuation = 1.f / (1.f + pointLights[i].posLin.w * distance + pointLights[i].colQuad.w * distance * distance);
		//float attenuation = 1.f / (1.f + pointLights[i].posLin.w * distance + 0.00011 * distance * distance);
		diffuse *= attenuation;
		specular *= attenuation;
		ambient += (diffuse + specular);
	}*/

	for(int i = 0; i < NR_SPOT_LIGHTS; ++i)
	{
		vec4 posRad = spotLights[i].posRad;
		vec4 dirInner = spotLights[i].dirInner;
		vec4 colOuter = spotLights[i].colOuter;
		vec4 linQuad = spotLights[i].linQuad;

		vec3 lightDir = normalize(posRad.xyz - wFragPos);  
		float theta = dot(lightDir, normalize(-dirInner.xyz));
		float epsilon =  dirInner.w - colOuter.w;
		float intensity = clamp((theta - colOuter.w) / epsilon, 0.0, 1.0); 

		//vec3 norm = normalize(normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = colOuter.xyz  * diff * albedo;  
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfwayDir), 0.0), 16); 
		vec3 specular = colOuter.xyz * spec * Specular;

		float distance = length(posRad.xyz - wFragPos);
		float attenuation = 1.f / (1.f + linQuad.x * distance + linQuad.y * distance * distance);
		diffuse *= attenuation;
		specular *= attenuation;

		diffuse  *= intensity;
		specular *= intensity;
		//ambient += diffuse + specular;

		vec4 fragPosLightSpace = lightMat * vec4(fragPos,1.f);
		//float lightSpaceFragDepth = length(posRad.xyz - fragPos);
		vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	    // Transform to [0,1] range
	    projCoords = projCoords * 0.5 + 0.5;
	    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	    //float closestDepth = texture(shadow, projCoords.xy).r; 
	    // Get depth of current fragment from light's perspective
	    float currentDepth = projCoords.z;
	    // Check whether current frag pos is in shadow
	    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);
	    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;      
    	//vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;   

    	const int pres = 2;

        float sshadow = 0.0;
		vec2 texelSize = 1.0 / textureSize(shadow, 0);
		for(int x = -pres; x <= pres; ++x)
		{
		    for(int y = -pres; y <= pres; ++y)
		    {
				float pcfDepth = texture(shadow, projCoords.xy + vec2(x, y) * texelSize).r; 
		        sshadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		    } 
		}
		sshadow /= ((2.f*pres) + 1) * ((2.f*pres) + 1);

		if(projCoords.z > 1.0)
        	sshadow = 0.0;

		ambient += (1.f - sshadow) * (diffuse + specular);

	}

	for(int i = 0; i < NR_DIRECT_LIGHTS; ++i)
	{
		//vec3 lightDir = normalize(-directLights[i].direction);
		//vec3 dir = -pos[0].xyz;
		vec3 dir = -vec3(-2.f,-2.f,-4.f);
		vec3 lightDir = normalize(dir.xyz);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse  = directLights[i].colour  * diff * albedo;
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 16); 
		vec3 specular = directLights[i].colour * spec * Specular;
	}

	//Emission
	//vec3 emission = vec3(texture(material.emit, TexCoord)) * material.emitStrength;

	//ambient *= pow(ssao,10.01);

	//float rangeCheck = smoothstep(1.0, 0.0, ssao);
	//ambient *= ssao;

	if(filmic)
	{
		//ambient *= vec3(1.f);
		vec3 curr = Uncharted2Tonemap(exposure*ambient);

		vec3 whiteScale = vec3(1.f)/Uncharted2Tonemap(vec3(W));
		vec3 colour = curr*whiteScale;
		outColour = vec4(pow(colour,vec3(1/2.2)), 1.f);
	}
	else
	{
	    const float gamma = 2.2;
	    vec3 hdrColor = ambient;

	    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	    mapped = pow(mapped, vec3(1.0 / gamma));
	  
	    outColour = vec4(mapped, 1.0);
	}

	/*float dd = texture(shadow, TexCoord).r;
	dd = LinearizeDepth(dd)-300.f;
	//outColour = vec4(vec3(ssao),1.f);
	if(dd > 0.1f && dd < 0.9f)
	{
		outColour = vec4(1.f,0.f,1.f,1.f);
	}
	else
	{
		outColour = vec4(vec3(dd),1.f);
	}*/
	//outColour = vec4(vec3(texture(shadow,TexCoord).r),1.f);
	//outColour = vec4(norm,1.f);
	//outColour = vec4(vec3(ssao),1.f);
};