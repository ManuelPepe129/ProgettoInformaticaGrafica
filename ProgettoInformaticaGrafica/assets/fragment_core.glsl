#version 330 core

struct Material{
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform int noTex;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

struct DirectionalLight{
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform DirectionalLight directionalLight;

#define MAX_POINT_LIGHTS 20
struct PointLight{
	vec3 position;

	// attenuation constants
	float c0;
	float c1;
	float c2;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

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

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPosition;
uniform Material material;

vec4 calcDirectionalLight(vec3 norm, vec3 viewDir, vec4 texDiff, vec4 texSpec);
vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 texDiff, vec4 texSpec);
vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 texDiff, vec4 texSpec);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPosition - FragPos);
	vec4 texDiff;
	vec4 texSpec;

	if(noTex == 1)
	{
		texDiff = material.diffuse;
		texSpec = material.specular;
	}
	else
	{
		texDiff = texture(diffuse0, TexCoord);
		texSpec = texture(specular0, TexCoord);
	}

	vec4 result;

	result = calcDirectionalLight(norm, viewDir, texDiff, texSpec);

	// point lights
	for(int i = 0; i < noPointLights; ++i)
	{
		result += calcPointLight(i, norm, viewDir, texDiff, texSpec);
	}

	// spot lights
	for(int i = 0; i < noSpotLights; ++i)
	{
		result += calcSpotLight(i, norm, viewDir, texDiff, texSpec);
	}

	FragColor = result;
}

vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 texDiff, vec4 texSpec)
{
	// ambient
	vec4 ambient = pointLights[idx].ambient * texDiff;

	// diffuse
	vec3 lightDir = normalize(pointLights[idx].position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f); // (L*N)
	vec4 diffuse = pointLights[idx].diffuse * texDiff * diff; // I_d*k_d*(L*N)

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0f); // (V*R)^n
	vec4 specular = pointLights[idx].specular * spec * texSpec; // I_s*k_s*(V*R)^n

	// light attenuation
	float dist = length(pointLights[idx].position - FragPos);
	float attenuation = 1.0f / (pointLights[idx].c0 + dist * pointLights[idx].c1 + (dist * dist) * pointLights[idx].c2);

	return (ambient + diffuse + specular) * attenuation;
}

vec4 calcDirectionalLight(vec3 norm, vec3 viewDir, vec4 texDiff, vec4 texSpec)
{
	// ambient
	vec4 ambient = directionalLight.ambient * texDiff;
	
	// diffuse
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec4 diffuse = directionalLight.diffuse * texDiff * diff;

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0f); // (V*R)^n
	vec4 specular = directionalLight.specular * spec * texSpec; // I_s*k_s*(V*R)^n

	return (ambient + diffuse + specular);
}

vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 texDiff, vec4 texSpec)
{
	vec3 lightDir = normalize(spotLights[idx].position - FragPos);
	float theta = dot(lightDir, normalize(-spotLights[idx].direction));

	// ambient
	vec4 ambient = spotLights[idx].ambient * texDiff;

	if(theta > spotLights[idx].outerCutOff) // > because using cosine and not degrees
	{
		// if in cutoff, render light

		// diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		vec4 diffuse = spotLights[idx].diffuse * (diff * texDiff);

		// specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess * 128.0f); // (V*R)^n
		vec4 specular = spotLights[idx].specular * spec * texSpec; // I_s*k_s*(V*R)^n

		float intensity = (theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff);
		intensity = clamp(intensity, 0.0f, 1.0f);
		diffuse *= intensity;
		specular *= intensity;

		float dist = length(spotLights[idx].position - FragPos);
		float attenuation = 1.0f / (spotLights[idx].c0 + dist * spotLights[idx].c1 + (dist * dist) * spotLights[idx].c2);

		return (ambient + diffuse + specular) * attenuation;
	}

	return ambient;
}

