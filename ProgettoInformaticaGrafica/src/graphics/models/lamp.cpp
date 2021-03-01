#include "lamp.h"

Lamp::Lamp(glm::vec3 pos, glm::vec3 size, glm::vec3 lightColor, float c0, float c1, float c2, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):
	lightColor(lightColor),
	pointLight({ pos, c0, c1, c2, ambient, diffuse, specular }), 
	Cube(pos, size, Material::white_plastic)
{
}

Lamp::Lamp()
{ }

void Lamp::render(Shader shader)
{
	// set light color
	shader.set3Float("lightColor", lightColor);

	Cube::render(shader);
}
