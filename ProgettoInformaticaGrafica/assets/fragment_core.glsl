#version 330 core

struct Material{
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

#define MAX_POINT_LIGHTS 20
struct PointLight{
	vec3 position;

	// attenuation constants
	float c0;
	float c1;
	float c2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

struct DirectionalLight{
	vec3 direction;

	// attenuation constants
	float c0;
	float c1;
	float c2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectionalLight directionalLight;

#define MAX_SPOT_LIGHTS 5
struct SpotLight{
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	// attenuation constants
	float c0;
	float c1;
	float c2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPosition;
uniform Material material;

vec3 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);
vec3 calcDirectionalLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);
vec3 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPosition - FragPos);
	vec3 diffMap = vec3(texture(material.diffuse, TexCoord));
	vec3 specMap = vec3(texture(material.specular, TexCoord));

	vec3 result;

	result = calcDirectionalLight(norm, viewDir, diffMap, specMap);

	// point lights
	for(int i = 0; i <noPointLights;++i)
	{
		result += calcPointLight(i, norm, viewDir, diffMap, specMap);
	}

	// spot lights
	for(int i = 0; i <noSpotLights; ++i)
	{
		result += calcSpotLight(i, norm, viewDir, diffMap, specMap);
	}

	FragColor = vec4(result, 1.0f);
}

vec3 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// ambient
	vec3 ambient = pointLights[idx].ambient * diffMap;

	// diffuse
	vec3 lightDir = normalize(pointLights[idx].position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f); // (L*N)
	vec3 diffuse = pointLights[idx].diffuse * diffMap * diff; // I_d*k_d*(L*N)

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0f); // (V*R)^n
	vec3 specular = pointLights[idx].specular * spec * specMap; // I_s*k_s*(V*R)^n

	// light attenuation
	float dist = length(pointLights[idx].position - FragPos);
	float attenuation = 1.0f / (pointLights[idx].c0 + dist * pointLights[idx].c1 + (dist * dist) * pointLights[idx].c2);

	return vec3(ambient + diffuse + specular) * attenuation;
}

vec3 calcDirectionalLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// ambient
	vec3 ambient = directionalLight.ambient * diffMap;
	
	// diffuse
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = directionalLight.diffuse * diffMap * diff;

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0f); // (V*R)^n
	vec3 specular = directionalLight.specular * spec * specMap; // I_s*k_s*(V*R)^n

	float dist = length(-directionalLight.direction);
	float attenuation = 1.0f / (directionalLight.c0 + dist * directionalLight.c1 + (dist * dist) * directionalLight.c2);

	return vec3(ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	vec3 lightDir = normalize(spotLights[idx].position - FragPos);
	float theta = dot(lightDir, normalize(-spotLights[idx].direction));

	// ambient
	vec3 ambient = spotLights[idx].ambient * diffMap;

	if(theta > spotLights[idx].outerCutOff) // > because using cosine and not degrees
	{
		// if in cutoff, render light

		// diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = spotLights[idx].diffuse * (diff * diffMap);

		// specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0f); // (V*R)^n
		vec3 specular = spotLights[idx].specular * spec * specMap; // I_s*k_s*(V*R)^n

		float intensity = (theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff);
		intensity = clamp(intensity, 0.0f, 1.0f);
		diffuse *= intensity;
		specular *= intensity;

		float dist = length(spotLights[idx].position - FragPos);
		float attenuation = 1.0f / (spotLights[idx].c0 + dist * spotLights[idx].c1 + (dist * dist) * spotLights[idx].c2);

		return vec3(ambient + diffuse + specular) * attenuation;
	}

	return ambient;
}

