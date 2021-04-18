#include "enemy.h"

Enemy::Enemy()
	: EntityBase("enemy", "enemy", nullptr)
{
	
}

Enemy::Enemy(const std::string& modelId, Scene* scene)
	:EntityBase("enemy", modelId, scene),
	respawnDelay(15.0f),
	respawnTimer(respawnDelay)
{ }

void Enemy::setPath(glm::vec3 start, glm::vec3 end, float speed)
{
	this->start = start;
	this->end = end;
	this->speed = speed;
	rigidBody->velocity = glm::normalize(end - start)*speed;
}

void Enemy::init(glm::vec3 size, float mass, glm::vec3 pos)
{
	EntityBase::init(size, mass, pos);
	setPath(start, end, speed);
}

void Enemy::update(double dt)
{
	if (rigidBody && !States::isActive(&rigidBody->state, INSTANCE_DEAD))
	{
		if (length(end - rigidBody->pos) <= 0.1f)
		{
			setPath(end, start, speed);
		}
	}
	else
	{
		respawnTimer -= dt;
		if (respawnTimer <= 0)
		{
			respawnTimer = respawnDelay;
			init(glm::vec3(1.0f), 1.0f, start);
		}
	}
	
}
