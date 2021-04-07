#pragma once

#include "../graphics/model.h"
#include "../scenes/scene.h"
#include <string>

class EntityBase
{
public:
	EntityBase(Scene& scene);

	virtual void setModel(Model& model);
	virtual void setModelShader(Shader& shader);

	virtual void init()=0;
	virtual void update(double dt)=0;
	virtual void render()=0;

protected:
	Model* model;
	Shader* shader;
	Scene& scene;

	//bool initialized;
};

