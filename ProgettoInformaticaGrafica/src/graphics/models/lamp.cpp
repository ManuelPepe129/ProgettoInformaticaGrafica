#include "lamp.h"

Lamp::Lamp(glm::vec3 pos, glm::vec3 size, glm::vec3 lightColor, float c0, float c1, float c2, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular):
	lightColor(lightColor),
	pointLight({ pos, c0, c1, c2, ambient, diffuse, specular }), 
	Cube(pos, size)
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
