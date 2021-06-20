#include "enemy.h"


Enemy::Enemy()
	: EntityBase("enemy", "enemy", nullptr),
	respawnDelay(15.0f),
	respawnTimer(respawnDelay),
	maxSpeed(10.0f),
	speed(2),
	shootDelay(2.0f),
	shootTimer(shootDelay)
{

}

Enemy::Enemy(const std::string& modelId, Scene* scene)
	: EntityBase("enemy", modelId, scene),
	respawnDelay(15.0f),
	respawnTimer(respawnDelay),
	maxSpeed(10.0f),
	speed(2.0f),
	shootDelay(2.0f),
	shootTimer(shootDelay)
{ }

void Enemy::setPath(glm::vec3 start, glm::vec3 end)
{
	this->start = start;
	this->end = end;
	this->speed = (speed > maxSpeed) ? maxSpeed : speed;
	rigidBody->velocity = glm::normalize(end - start) * speed;
	glm::vec3 v = rigidBody->velocity;
	float yaw = atan2(rigidBody->velocity.x, rigidBody->velocity.z);
	rigidBody->rot = glm::vec3(0.0f, glm::half_pi<float>() - yaw, 0.0f);
}

void Enemy::init(glm::vec3 size, float mass, glm::vec3 pos)
{
	EntityBase::init(size, mass, pos);
	shootTimer = shootDelay;
}

void Enemy::update(double dt)
{
	if (rigidBody && !States::isActive(&rigidBody->state, INSTANCE_DEAD))
	{
		// if the enemy is alive
		if (length(end - rigidBody->pos) <= 0.1f)
		{
			setPath(end, start);
		}

		if (scene && scene->getActiveCamera())
		{
			Camera* camera = scene->getActiveCamera();
			float dist = glm::length(rigidBody->pos - camera->cameraPos);
			if (dist <= EnemySeeDistance)
			{
				//std::cout << "Player nearby\n";
				if (shootTimer <= 0.0f)
				{
					//std::cout << "Shooting\n";
					RigidBody* projectile_rb = scene->generateInstance("projectile", glm::vec3(.1f), 0.50f, rigidBody->pos);
					if (projectile_rb) {
						// instance generated
						projectile_rb->transferEnergy(80.0f, glm::normalize(camera->cameraPos + glm::vec3(0.0f, 0.10f, 0.0f) - rigidBody->pos));
						projectile_rb->applyAcceleration(Environment::gravitationalAcceleration);
					}
					shootTimer = shootDelay;
				}
				else
				{
					shootTimer -= dt;
				}
			}
			else {
				//std::cout << "Player too far\n";
			}
		}
	}
	else
	{
		// if the enemy is alive
		// decrement respawn time
		respawnTimer -= dt;
		if (respawnTimer <= 0)
		{
			// respawn
			respawnTimer = respawnDelay;
			init(glm::vec3(1.0f), 1.0f, start);
			speed++;
			setPath(end, start);
		}
	}

}
