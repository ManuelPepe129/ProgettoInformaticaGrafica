#include "rigidbody.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

bool RigidBody::operator==(RigidBody rb) {
	return instanceId == rb.instanceId;
}

bool RigidBody::operator==(std::string id) {
	return instanceId == id;
}

RigidBody::RigidBody()
	: state(0)
{ }

RigidBody::RigidBody(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot)
	: modelId(modelId), size(size), mass(mass), pos(pos), rot(rot), velocity(glm::vec3(0.0f)), acceleration(glm::vec3(0.0f)), state(0)
{
	update(0.0f);
}

void RigidBody::update(float dt)
{
	pos += velocity * dt + 0.5f * acceleration * (dt * dt);
	velocity += acceleration * dt;

	// calculate rotation matrix
	glm::mat4 rotMat = glm::toMat4(glm::quat(rot));

	// model = trans * rot * scale = T * R * S
	model = glm::translate(glm::mat4(1.0f), pos); // M = I * T = T
	model = model * rotMat; // M = M * R = T * R
	model = glm::scale(model, size); // M = M * S = T * R * S

	normalModel = glm::mat3(glm::transpose(glm::inverse(model)));
}

void RigidBody::applyForce(glm::vec3 force)
{
	acceleration += force / mass;
}

void RigidBody::applyForce(glm::vec3 direction, float magnitude)
{
	applyForce(direction * magnitude);
}

void RigidBody::applyAcceleration(glm::vec3 a)
{
	acceleration += a;
}

void RigidBody::applyAcceleration(glm::vec3 direction, float magnitude)
{
	applyAcceleration(direction * magnitude);
}

void RigidBody::applyImpulse(glm::vec3 force, float dt)
{
	velocity += force / mass * dt;
}

void RigidBody::applyImpulse(glm::vec3 direction, float magnitude, float dt)
{
	applyImpulse(direction * magnitude, dt);
}

void RigidBody::transferEnergy(float joules, glm::vec3 direction)
{
	if (joules == 0) {
		return;
	}

	// comes from formula: KE = 1/2 * m * v^2
	glm::vec3 deltaV = sqrt(2 * abs(joules) / mass) * direction;

	velocity += joules > 0 ? deltaV : -deltaV;
}