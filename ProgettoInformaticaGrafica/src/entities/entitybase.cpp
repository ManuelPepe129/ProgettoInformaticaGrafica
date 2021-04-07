#include "entitybase.h"

EntityBase::EntityBase(Scene& scene)
	: scene(scene)
{

}

void EntityBase::setModel(Model& model)
{
	this->model = &model;
	scene.registerModel(&model);
	scene.generateInstance(model.id, glm::vec3(1.0f), 1.0f, glm::vec3(0.0f));
}

void EntityBase::setModelShader(Shader& shader)
{
	this->shader = &shader;
}

void EntityBase::render()
{
	if (model && shader)
	{
		scene.renderShader(*shader);
		model->render(*shader, &scene);
	}
}
