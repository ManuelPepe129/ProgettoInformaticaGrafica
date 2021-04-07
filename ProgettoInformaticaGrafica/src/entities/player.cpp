#include "player.h"

#include "../graphics/models/cube.hpp"

Player::Player(Scene& scene)
	:EntityBase(scene)
{
}

void Player::setPlayerCamera(Camera* camera)
{
	this->camera = camera;
	cameraBR = new BoundingRegion(camera->cameraPos, 1.0f);
}

void Player::init()
{
	std::cout << "Player init\n";
	// Model* cube = new Cube();
	// setModel(*cube);
	// cube->size = glm::vec3(10.0f);
}

void Player::update(double dt)
{
	if (camera) {
		// player camera exists
		camera->hasMoved = false;

		// set camera direction
		double mouseDX = Mouse::getDX();
		double mouseDY = Mouse::getDY();
		if (mouseDX != 0.0 || mouseDY != 0)
		{
			camera->updateCameraDirection(mouseDX, mouseDY);
		}

		// set camera zoom
		//cameras[activeCamera]->updateCameraZoom(Mouse::getScrollDY());

		// set camera pos
		if (Keyboard::key(GLFW_KEY_W)) {
			camera->updateCameraPos(CameraDirection::FORWARD, dt);
			checkCollisions();
		}
		if (Keyboard::key(GLFW_KEY_S)) {
			camera->updateCameraPos(CameraDirection::BACKWARD, dt);
			cameraBR->center = camera->cameraPos;
			scene.octree->checkCollisionsSelf(*cameraBR);
			scene.octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_D)) {
			camera->updateCameraPos(CameraDirection::RIGHT, dt);
			cameraBR->center = camera->cameraPos;
			scene.octree->checkCollisionsSelf(*cameraBR);
			scene.octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_A)) {
			camera->updateCameraPos(CameraDirection::LEFT, dt);
			cameraBR->center = camera->cameraPos;
			scene.octree->checkCollisionsSelf(*cameraBR);
			scene.octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_SPACE)) {
			camera->updateCameraPos(CameraDirection::UP, dt);
			cameraBR->center = camera->cameraPos;
			scene.octree->checkCollisionsSelf(*cameraBR);
			scene.octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
			camera->updateCameraPos(CameraDirection::DOWN, dt);
			cameraBR->center = camera->cameraPos;
			scene.octree->checkCollisionsSelf(*cameraBR);
			scene.octree->checkCollisionsChildren(*cameraBR);
		}
	}
}

void Player::render()
{
	EntityBase::render();
}

bool Player::checkCollisions()
{
	cameraBR->center = camera->cameraPos;
	scene.octree->checkCollisionsSelf(*cameraBR);
	scene.octree->checkCollisionsChildren(*cameraBR);
	return false;
}
