#include "camera.h"

Camera::Camera(glm::vec3 position)
	: cameraPos(position),
	worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	yaw(0.0f),
	pitch(0.0f),
	speed(2.5f),
	sensitivity(1.0f),
	zoom(45.0f),
	cameraFront(glm::vec3(1.0f, 0.0f, 0.0f)),
	hasMoved(false)
{
	updateCameraVectors();
}

// mouse movement
void Camera::updateCameraDirection(double dx, double dy) {
	yaw += dx;
	pitch += dy;

	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
	}
	else if (yaw < -360.0f)
	{
		yaw += 360.0f;
	}

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	else if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::updateCameraPos(CameraDirection direction, double dt) {
	float velocity = (float)dt * speed;

	lastDirections.push_back(direction);

	switch (direction) {
	case CameraDirection::FORWARD:
		cameraPos += cameraFront * velocity;
		break;
	case CameraDirection::BACKWARD:
		cameraPos -= cameraFront * velocity;
		break;
	case CameraDirection::RIGHT:
		cameraPos += cameraRight * velocity;
		break;
	case CameraDirection::LEFT:
		cameraPos -= cameraRight * velocity;
		break;
	case CameraDirection::UP:
		cameraPos += cameraUp * velocity;
		break;
	case CameraDirection::DOWN:
		cameraPos -= cameraUp * velocity;
		break;
	}

	// TODO: scommentare
#if !DEBUG
	cameraPos.y = 0.0f;
#endif

	hasMoved = true;
}

void Camera::revertCameraPos(double dt)
{
	float velocity = (float)dt * speed;
	for (CameraDirection direction : lastDirections) {
		switch (direction) {
		case CameraDirection::FORWARD:
			cameraPos -= cameraFront * velocity;
			break;
		case CameraDirection::BACKWARD:
			cameraPos += cameraFront * velocity;
			break;
		case CameraDirection::RIGHT:
			cameraPos -= cameraRight * velocity;
			break;
		case CameraDirection::LEFT:
			cameraPos += cameraRight * velocity;
			break;
		case CameraDirection::UP:
			cameraPos -= cameraUp * velocity;
			break;
		case CameraDirection::DOWN:
			cameraPos += cameraUp * velocity;
			break;
		}
	}
	lastDirections.clear();
#if !DEBUG
	cameraPos.y = 0.0f;
#endif // !DEBUG
	hasMoved = false;
	
}

void Camera::updateCameraZoom(double dy) {
	if (zoom >= 1.0f && zoom <= 45.0f) {
		zoom -= dy;
	}
	else if (zoom < 1.0f) {
		zoom = 1.0f;
	}
	else { // > 45.0f
		zoom = 45.0f;
	}
}

void Camera::resetDirections()
{
	lastDirections.clear();
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

float Camera::getZoom() {
	return zoom;
}
	
void Camera::updateCameraVectors() {
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}