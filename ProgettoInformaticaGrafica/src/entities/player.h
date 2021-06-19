#pragma once

#include "entitybase.h"

#define DEBUG 0

class Player : public EntityBase
{
public:
	Player(const std::string& name, Scene* scene);

	void setPlayerCamera(Camera* camera);

	virtual void init(glm::vec3 size, float mass, glm::vec3 pos);
	virtual void update(double dt);

private: 
	Camera* camera;
};

