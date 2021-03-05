#pragma once

#include "../model.h"
#include "../modelArray.hpp"

class Sphere : public Model
{
public:
	Sphere(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));

	void init();
};

class SphereArray : public ModelArray<Sphere> {
public:
	void init();
};

