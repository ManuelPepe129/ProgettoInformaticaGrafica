#pragma once

#include "entitybase.h"

class Player : public EntityBase
{
public:
	Player(Scene& scene);

	void setPlayerCamera(Camera* camera);

	virtual void init();
	virtual void update(double dt);
	virtual void render();

private: 
	Camera* camera;
	BoundingRegion* cameraBR;

	bool checkCollisions();
};

