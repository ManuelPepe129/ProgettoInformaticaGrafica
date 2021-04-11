#pragma once

#include "../graphics/model.h"
#include "../scenes/scene.h"
#include <string>

class EntityBase
{
public:

	EntityBase(const std::string& name, const std::string& modelId, Scene* scene);

	virtual void init(glm::vec3 size, float mass, glm::vec3 pos);
	virtual void update(double dt)=0;

	//virtual void onCollide(EntityBase* other) = 0;

//protected:

	const std::string name;
	const std::string modelId;
	Scene* scene;

	RigidBody* rigidBody;

	//bool initialized;
};

