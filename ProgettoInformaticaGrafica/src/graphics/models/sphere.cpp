#include "sphere.h"

Sphere::Sphere(glm::vec3 pos , glm::vec3 size)
	: Model(BoundTypes::SPHERE, pos, size, true)
{
	rb.mass = 1.0f;
}

void Sphere::init()
{
	loadModel("assets/models/sphere/scene.gltf");
}

void SphereArray::init()
{
	model = Sphere(glm::vec3(0.0f), glm::vec3(0.25f));
	ModelArray::init();
}
