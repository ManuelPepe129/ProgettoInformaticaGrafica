#include "player.h"

Player::Player(const std::string& name ,Scene* scene)
	:EntityBase(name, "",scene)
{
}

void Player::setPlayerCamera(Camera* camera)
{
	this->camera = camera;
	//cameraBR = new BoundingRegion(camera->cameraPos, 1.0f);
}

void Player::init(glm::vec3 size, float mass, glm::vec3 pos)
{
	std::cerr << "Initialize with setPlayerCamera\n";
}

void Player::update(double dt)
{
	if (camera)
	{
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
		if (Keyboard::key(GLFW_KEY_W)) 
		{
			camera->updateCameraPos(CameraDirection::FORWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_S)) 
		{
			camera->updateCameraPos(CameraDirection::BACKWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_D)) 
		{
			camera->updateCameraPos(CameraDirection::RIGHT, dt);
		}
		if (Keyboard::key(GLFW_KEY_A))
		{
			camera->updateCameraPos(CameraDirection::LEFT, dt);
		}

#ifdef DEBUG
		if (Keyboard::key(GLFW_KEY_SPACE)) 
		{
			camera->updateCameraPos(CameraDirection::UP, dt);
		}
		if (Keyboard::key(GLFW_KEY_LEFT_SHIFT))
		{
			camera->updateCameraPos(CameraDirection::DOWN, dt);
		}
#endif
		
	}
}

