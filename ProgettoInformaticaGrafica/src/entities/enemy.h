#pragma once

#include "entitybase.h"
#include "../physics/environment.h"

#define EnemySeeDistance 15.0f

class Enemy :
    public EntityBase
{
public:
	Enemy();
	Enemy(const std::string& modelId, Scene* scene);

	void setPath(glm::vec3 start, glm::vec3 end);

	virtual void init(glm::vec3 size, float mass, glm::vec3 pos);
	virtual void update(double dt) ;

protected:
	glm::vec3 start;
	glm::vec3 end;

	
	float speed;
	float maxSpeed;
	float respawnDelay;
	float respawnTimer;

	float shootDelay;
	float shootTimer;
};

