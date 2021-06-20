#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define DEBUG 0

enum class CameraDirection {
	NONE = 0,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera {
public:
	bool hasMoved;

	glm::vec3 cameraPos;

	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::vec3 worldUp;

	float yaw; // x-axis
	float pitch; // y-axis
	float speed;
	float sensitivity;


	Camera(glm::vec3 position = glm::vec3(0.0f));

	void updateCameraDirection(double dx, double dy); // moving mouse
	void updateCameraPos(CameraDirection direction, double dt); // keyboard input
	void revertCameraPos(double dt); // keyboard input
	void updateCameraZoom(double dy); // scroll wheel

	void resetDirections();

	glm::mat4 getViewMatrix();
	float getZoom();

private:
	void updateCameraVectors();

	std::vector<CameraDirection> lastDirections;

	float zoom;
};

#endif