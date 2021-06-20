#include "player.h"

Player::Player(const std::string& name ,Scene* scene)
	:EntityBase(name, "",scene), joystick(Joystick(0)), camera(nullptr), axeThrown(false)
{
	//joystick.update();
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

		joystick.update();

		/* Joystick Handler */
		if (joystick.isPresent())
		{
			float lx = joystick.axesState(GLFW_JOYSTICK_AXES_LEFT_STICK_X);
			float ly = -joystick.axesState(GLFW_JOYSTICK_AXES_LEFT_STICK_Y);

			if (lx > 0.05f)
			{
				camera->updateCameraPos(CameraDirection::RIGHT, dt);
			}
			else if (lx < -0.05f)
			{
				camera->updateCameraPos(CameraDirection::LEFT, dt);
			}
			if (ly > 0.05f)
			{
				camera->updateCameraPos(CameraDirection::FORWARD, dt);
			}
			else if (ly < -0.05f)
			{
				camera->updateCameraPos(CameraDirection::BACKWARD, dt);
			}

#if DEBUG
			if (Keyboard::key(GLFW_KEY_SPACE))
			{
				camera->updateCameraPos(CameraDirection::UP, dt);
			}
			if (Keyboard::key(GLFW_KEY_LEFT_SHIFT))
			{
				camera->updateCameraPos(CameraDirection::DOWN, dt);
			}
#endif

			/*
			std::cout << "Left Stick X Axis: " << joystick.axesState(0) << '\n';
			std::cout << "Left Stick Y Axis: " << joystick.axesState(1) << '\n';
			std::cout << "Right Stick X Axis: " << joystick.axesState(2) << '\n';
			std::cout << "Right Stick Y Axis: " << joystick.axesState(3) << '\n';
			std::cout << "Left Trigger/L2: " << joystick.axesState(4) << '\n';
			std::cout << "Right Trigger/R2: " << joystick.axesState(5) << '\n';
			std::cout << '\n' << '\n';
			*/

			float rx = joystick.axesState(GLFW_JOYSTICK_AXES_RIGHT_STICK_X);
			float ry = -joystick.axesState(GLFW_JOYSTICK_AXES_RIGHT_STICK_Y);
			if (std::abs(rx) > 0.5f || std::abs(ry) > 0.5f)
			{
				if (std::abs(rx) <= 0.5f)
					rx = 0.0f;
				if (std::abs(ry) <= 0.5f)
					ry = 0.0f;
				camera->updateCameraDirection(rx * 1.5f, ry * 1.5f);
			}

			float rt = joystick.axesState(GLFW_JOYSTICK_AXES_LEFT_TRIGGER);

			if (rt > 0.5f)
			{
				if (!axeThrown)
				{
					throwAxe();
					axeThrown = true;
				}
			}
			else
			{
				axeThrown = false;
			}
		}
		else
		{
			axeThrown = false;
		}

		


		
	}
}

void Player::throwAxe() {
	if (scene)
	{
		if (scene->getAxes() > 0)
		{
			RigidBody* rb = scene->generateInstance("axe", glm::vec3(.50f), 1.0f, camera->cameraPos, glm::vec3(0.0, glm::radians(-camera->yaw + 90.0), 0.0));
			scene->removeAxe();
			if (rb) {
				// instance generated
				rb->transferEnergy(100.0f, camera->cameraFront);
				rb->applyAcceleration(Environment::gravitationalAcceleration);
			}
		}
		else
		{
			std::cout << "not enough axes\n";
		}

	}
}