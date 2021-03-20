#ifndef LAMP_HPP
#define LAMP_HPP

#include "cube.hpp"
#include "../material.h"
#include "../shader.h"
#include "../light.h"
#include "modelarray.hpp"

#include <glm/glm.hpp>

class Lamp : public Cube {
public:
	glm::vec3 lightColor;

	PointLight pointLight;

	Lamp(glm::vec3 lightColor = glm::vec3(1.0f),
		glm::vec4 ambient = glm::vec4(1.0f),
		glm::vec4 diffuse = glm::vec4(1.0f),
		glm::vec4 specular = glm::vec4(1.0f),
		float k0 = 1.0f,
		float k1 = 0.07f,
		float k2 = 0.017f,
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 size = glm::vec3(1.0f))
		: lightColor(lightColor),
		pointLight({ pos, k0, k1, k2, ambient, diffuse, specular }),
		Cube(pos, size) {}

	void render(Shader shader, float dt, Box *box, bool setModel = true, bool doRender = true) {
		// set light color
		shader.set3Float("lightColor", lightColor);

		Cube::render(shader, dt, box, setModel, doRender);
	}

	void render(Shader shader, Box* box, bool setModel = true, bool doRender = true) {
		// set light color
		shader.set3Float("lightColor", lightColor);

		Cube::render(shader,box, setModel, doRender);
	}
};

class LampArray : public ModelArray<Lamp> {
public:
	std::vector<PointLight> lightInstances;

	void init() {
		model = Lamp(glm::vec3(1.0f),
			glm::vec4(0.05f, 0.05f, 0.05f, 1.0f), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), glm::vec4(1.0f),
			1.0f, 0.07f, 0.032f,
			glm::vec3(0.0f), glm::vec3(0.25f));
		ModelArray::init();
	}

	void render(Shader shader, float dt, Box *box) {
		positions.clear();
		sizes.clear();

		for (PointLight& pl : lightInstances) {
			positions.push_back(pl.position);
			sizes.push_back(model.size);
		}

		ModelArray::render(shader, dt, box, false);
	}

	void render(Shader shader, Box* box) {
		positions.clear();
		sizes.clear();

		for (PointLight& pl : lightInstances) {
			positions.push_back(pl.position);
			sizes.push_back(model.size);
		}

		ModelArray::render(shader,box, false);
	}
};

#endif