#pragma once

#include "../model.h"
#include "../material.h"

class Cube : public Model
{
public:
	Cube(glm::vec3 pos, glm::vec3 size, Material material);

	void init();

	void render(Shader shader);

	glm::vec3 pos;
	glm::vec3 size;

	Material material;
};

