#include "enemy.h"


Enemy::Enemy()
	: EntityBase("enemy", "enemy", nullptr),
	respawnDelay(15.0f),
	respawnTimer(respawnDelay),
	maxSpeed(10.0f),
	speed(2)
{

}

Enemy::Enemy(const std::string& modelId, Scene* scene)
	: EntityBase("enemy", modelId, scene),
	respawnDelay(15.0f),
	respawnTimer(respawnDelay),
	maxSpeed(10.0f),
	speed(2.0f)
{ }

void Enemy::setPath(glm::vec3 start, glm::vec3 end)
{
	this->start = start;
	this->end = end;
	this->speed = (speed > maxSpeed) ? maxSpeed : speed;
	rigidBody->velocity = glm::normalize(end - start) * speed;
	glm::vec3 v = rigidBody->velocity;
	std::cout << "[x: " << v.x << ", y: " << v.y
		<< ", z:" << v.z << "]\n";
	float yaw = atan2(rigidBody->velocity.x, rigidBody->velocity.z);
	std::cout << "yaw: " << yaw << '\n';
	rigidBody->rot = glm::vec3(0.0f, glm::half_pi<float>() - yaw, 0.0f);
}

void Enemy::init(glm::vec3 size, float mass, glm::vec3 pos)
{
	EntityBase::init(size, mass, pos);
}

void Enemy::update(double dt)
{
	if (rigidBody && !States::isActive(&rigidBody->state, INSTANCE_DEAD))
	{
		if (length(end - rigidBody->pos) <= 0.1f)
		{
			setPath(end, start);
		}
	}
	else
	{
		respawnTimer -= dt;
		if (respawnTimer <= 0)
		{
			respawnTimer = respawnDelay;
			init(glm::vec3(1.0f), 1.0f, start);
			speed++;
			setPath(end, start);
		}
	}

}
