#include "lamp.h"

Lamp::Lamp(glm::vec3 lightColor,
	glm::vec4 ambient,
	glm::vec4 diffuse,
	glm::vec4 specular,
	float c0,
	float c1,
	float c2,
	glm::vec3 pos,
	glm::vec3 size)
	: lightColor(lightColor),
	pointLight({ pos, c0, c1, c2, ambient, diffuse, specular }),
	Cube(pos, size) {}

/*
Lamp::Lamp()
{ }
*/

void Lamp::render(Shader shader, float dt, Box* box, bool setModel, bool doRender)
{
	// set light color
	shader.set3Float("lightColor", lightColor);

	Cube::render(shader, dt, box, setModel, doRender);
}

void LampArray::init() {
	model = Lamp(glm::vec3(1.0f),
		glm::vec4(0.05f, 0.05f, 0.05f, 1.0f), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), glm::vec4(1.0f),
		1.0f, 0.07f, 0.032f,
		glm::vec3(0.0f), glm::vec3(0.25f));
	ModelArray::init();
}

void LampArray::render(Shader shader, float dt, Box* box) {
	positions.clear();
	sizes.clear();

	for (PointLight& pl : lightInstances) {
		positions.push_back(pl.position);
		sizes.push_back(model.size);
	}

	ModelArray::render(shader, dt, box, false);
}
