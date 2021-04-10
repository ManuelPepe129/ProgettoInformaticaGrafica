#include "entitybase.h"

EntityBase::EntityBase(const std::string& name, const std::string& modelId, Scene* scene)
	: name(name),modelId(modelId), scene(scene)
{

}

void EntityBase::init(glm::vec3 size, float mass, glm::vec3 pos)
{
	rigidBody = scene->generateInstance(modelId, size, mass, pos);
}

