#include "enemy.h"

Enemy::Enemy(const std::string& modelId, Scene& scene)
	:EntityBase("enemy", modelId, scene), t(0.0f)
{ }

void Enemy::setPath(glm::vec3 start, glm::vec3 end, float speed)
{
	this->start = start;
	this->end = end;
	this->speed = speed;
	rigidBody->velocity = glm::normalize(end - start);
}

void Enemy::init(glm::vec3 size, float mass, glm::vec3 pos)
{
	EntityBase::init(size, mass, pos);
}

void Enemy::update(double dt)
{
	/*
	t = t + dt / time > 1.0f ? 1.0f : t + dt / time;
	rigidBody->pos = Tween::linear(start, end, t);
	if (t == 1.0f)
	{
		t = 0.0f;
		glm::vec3 tmp = start;
		start = end;
		end = tmp;
	}
	*/
	if (length(end - rigidBody->pos) <= 0.1f)
	{
		setPath(end, start, speed);
	}
}
