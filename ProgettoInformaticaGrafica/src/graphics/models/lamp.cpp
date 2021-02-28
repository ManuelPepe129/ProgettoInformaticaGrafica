#include "lamp.h"

Lamp::Lamp(glm::vec3 pos, glm::vec3 size, glm::vec3 lightColor, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):
	lightColor(lightColor), ambient(ambient), diffuse(diffuse), specular(specular), Cube(pos, size, Material::white_plastic)
{
}

void Lamp::render(Shader shader)
{
	// set light color
	shader.set3Float("lightColor", lightColor);

	Cube::render(shader);
}
