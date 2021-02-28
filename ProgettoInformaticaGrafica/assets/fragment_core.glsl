#version 330 core

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform Material material;
uniform Light light;
uniform vec3 viewPosition;

void main()
{
	// ambient
	vec3 ambient = light.ambient * material.ambient; // I_a*k_a

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f); // (L*N)
	vec3 diffuse = light.diffuse * material.diffuse * diff; // I_d*k_d*(L*N)

	// specular
	vec3 viewDir = normalize(viewPosition - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess*128.0f); // (V*R)^n
	vec3 specular = light.specular * (spec * material.specular); // I_s*k_s*(V*R)^n

	FragColor = vec4(vec3(ambient + diffuse + specular), 1.0f);
}