#pragma once

#include "../model.h"

class Sphere : public Model {
public:
	Sphere()
		: Model("sphere", BoundTypes::SPHERE, NO_TEX | DYNAMIC)
	{

	}

	void init() {
		loadModel("assets/models/sphere/scene.gltf");
	}
};
