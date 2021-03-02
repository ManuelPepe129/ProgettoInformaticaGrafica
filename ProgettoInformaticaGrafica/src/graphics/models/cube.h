#pragma once

#include "../model.h"
#include "../material.h"

class Cube : public Model
{
public:
	Cube(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));

	void init();

	// void render(Shader shader);

	glm::vec3 pos;
	glm::vec3 size;

	Material material;
};

