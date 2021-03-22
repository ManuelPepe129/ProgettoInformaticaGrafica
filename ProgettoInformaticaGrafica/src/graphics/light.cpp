#include "light.h"

DirLight::DirLight(glm::vec3 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular)
	: direction(direction), ambient(ambient), diffuse(diffuse), specular(specular)
{
}

void DirLight::render(Shader shader) {
	shader.set3Float("dirLight.direction", direction);
	shader.set4Float("dirLight.ambient", ambient);
	shader.set4Float("dirLight.diffuse", diffuse);
	shader.set4Float("dirLight.specular", specular);
}

PointLight::PointLight(glm::vec3 position, float k0, float k1, float k2, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular)
	: position(position), k0(k0), k1(k1), k2(k2), ambient(ambient), diffuse(diffuse), specular(specular)
{
}

void PointLight::render(Shader shader, int idx) {
	std::string name = "pointLights[" + std::to_string(idx) + "]";
	shader.set3Float(name + ".position", position);

	shader.setFloat(name + ".k0", k0);
	shader.setFloat(name + ".k1", k1);
	shader.setFloat(name + ".k2", k2);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);
}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff, float k0, float k1, float k2, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular)
	:position(position), direction(direction), cutOff(cutOff), outerCutOff(outerCutOff), k0(k0), k1(k1), k2(k2), ambient(ambient), diffuse(diffuse), specular(specular)
{
}

void SpotLight::render(Shader shader, int idx) {
	std::string name = "spotLights[" + std::to_string(idx) + "]";
	shader.set3Float(name + ".position", position);
	shader.set3Float(name + ".direction", direction);

	shader.setFloat(name + ".cutOff", cutOff);
	shader.setFloat(name + ".outerCutOff", outerCutOff);

	shader.setFloat(name + ".k0", k0);
	shader.setFloat(name + ".k1", k1);
	shader.setFloat(name + ".k2", k2);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);
}