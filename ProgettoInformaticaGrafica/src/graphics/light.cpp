#include "light.h"

void PointLight::render(Shader shader, int idx)
{
	std::string name = "pointLights["+std::to_string(idx)+"]";

	shader.set3Float(name + ".position", position);

	shader.setFloat(name + ".c0", c0);
	shader.setFloat(name + ".c1", c1);
	shader.setFloat(name + ".c2", c2);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);
}

void DirectionalLight::render(Shader shader)
{
	std::string name = "directionalLight";

	shader.set3Float(name + ".direction", direction);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);
}

void SpotLight::render(Shader shader, int idx)
{
	std::string name = "spotLights[" + std::to_string(idx) + "]";

	shader.set3Float(name + ".direction", direction);
	shader.set3Float(name + ".position", position);

	shader.setFloat(name + ".cutOff", cutOff);
	shader.setFloat(name + ".outerCutOff", outerCutOff);

	shader.setFloat(name + ".c0", c0);
	shader.setFloat(name + ".c1", c1);
	shader.setFloat(name + ".c2", c2);

	shader.set4Float(name + ".ambient", ambient);
	shader.set4Float(name + ".diffuse", diffuse);
	shader.set4Float(name + ".specular", specular);
}
