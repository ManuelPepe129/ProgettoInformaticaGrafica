#pragma once
#include "entitybase.h"

class Enemy :
    public EntityBase
{
public:
	Enemy();
	Enemy(const std::string& modelId, Scene* scene);

	void setPath(glm::vec3 start, glm::vec3 end, float speed = 10.0f);

	virtual void init(glm::vec3 size, float mass, glm::vec3 pos);
	virtual void update(double dt) ;

protected:
	glm::vec3 start;
	glm::vec3 end;

	float t;
	float speed;
};

