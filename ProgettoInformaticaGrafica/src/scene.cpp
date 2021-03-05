#include "scene.h"

unsigned int Scene::SCR_WIDTH = 0;
unsigned int Scene::SCR_HEIGHT = 0;

void Scene::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Scene::SCR_WIDTH = width;
	Scene::SCR_HEIGHT = height;
}

Scene::Scene(int glfwVersionMajor, int glfWVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: glfwVersionMajor(glfwVersionMajor), glfWVersionMinor(glfWVersionMinor), title(title),
	activeCamera(-1),
	activePointLights(0), activeSpotLights(0)
{
	Scene::SCR_WIDTH = scrWidth;
	Scene::SCR_HEIGHT = scrHeight;

	setWindowColor(0.1f, 0.15f, 0.15f, 1.0f);
}

bool Scene::init()
{
	glfwInit();

	// set version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfWVersionMinor);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

# ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COPMPAT, GL_TRUE);
#endif

	// initialize window
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
	if (window == NULL)
	{
		return false;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();

		return false;
	}

	//setup screen
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	/*
	*	callbacks 
	*/

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	
	glfwSetKeyCallback(window, Keyboard::keyCallback);

	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);

	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);

	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	/*
	*	set rendering parameters
	*/

	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor

	return true;
}

void Scene::processInput(float dt)
{
	if (activeCamera != -1 && activeCamera < cameras.size())
	{
		double dx = Mouse::getDX(), dy = Mouse::getDY();
		if (dx != 0 || dy != 0)
		{
			cameras[activeCamera]->updateCameraDirection(dx, dy);
		}

		/* move camera */

		if (Keyboard::key(GLFW_KEY_W))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, dt);
		}

		if (Keyboard::key(GLFW_KEY_S))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, dt);
		}

		if (Keyboard::key(GLFW_KEY_A))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, dt);
		}

		if (Keyboard::key(GLFW_KEY_D))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, dt);
		}

		if (Keyboard::key(GLFW_KEY_SPACE))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::UP, dt);
		}

		if (Keyboard::key(GLFW_KEY_LEFT_SHIFT))
		{
			cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, dt);
		}

		// set matrices
		view = cameras[activeCamera]->getViewMatrix();
		projection = glm::perspective(glm::radians(cameras[activeCamera]->getCameraZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		cameraPos = cameras[activeCamera]->getCameraPos();
	}


	/*
	mainJ.update();

	float lx = mainJ.axesState(GLFW_JOYSTICK_AXES_LEFT_STICK_X);
	float ly = -mainJ.axesState(GLFW_JOYSTICK_AXES_LEFT_STICK_Y);


	// handle joystick dead zone
	if (std::abs(lx) > 0.5f)
	{
		//transform = glm::translate(transform, glm::vec3(lx / 10.0f, 0.0f, 0.0f));
		x += lx/10.0f;
	}

	if (std::abs(ly) > 0.5f)
	{
		transform = glm::translate(transform, glm::vec3(0.0f, ly / 10.0f,0.0f));
		y += ly / 10.0f;
	}

	float rt = mainJ.axesState(GLFW_JOYSTICK_AXES_RIGHT_TRIGGER) / 2.0f + 0.5f;
	if (rt > 0.5f)
	{
		transform = glm::translate(transform, glm::vec3(1+rt / 10.0f, 1+rt / 10.0f, 0.0f));
	}

	float lt = mainJ.axesState(GLFW_JOYSTICK_AXES_LEFT_TRIGGER) / 2.0f + 0.5f;
	if (lt > 0.5f)
	{
		transform = glm::translate(transform, glm::vec3(1-lt / 10.0f, 1-lt / 10.0f, 0.0f));
	}
	*/
}

void Scene::update()
{
	glClearColor(bg[0],bg[1],bg[2],bg[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::newFrame()
{
	// send new frame to window;
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Scene::render(Shader shader, bool applyLighting)
{
	// activate shader
	shader.activate();

	// set camera values
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.set3Float("viewPos", cameraPos);

	// lighting
	if (applyLighting)
	{
		// point lights
		unsigned int noLights = pointLights.size();
		unsigned int noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; ++i)
		{
			if (States::isActive(&activePointLights, i)) {
				//i'th point light is active
				pointLights[i]->render(shader, noActiveLights++);
			}
		}
		shader.setInt("noPointLights", noActiveLights);

		// spot lights
		noLights = spotLights.size();
		noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; ++i)
		{
			if (States::isActive(&activeSpotLights, i)) {
				//i'th spot light is active
				pointLights[i]->render(shader, noActiveLights++);
			}
		}
		shader.setInt("noSpotLights", noActiveLights);

		// directional light
		directionalLight->render(shader);
	}
}

void Scene::cleanup()
{
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
}

bool Scene::shouldClose()
{
	return glfwWindowShouldClose(window);
}

Camera* Scene::getActiveCamera()
{
	if (activeCamera == -1 || activeCamera >= cameras.size()) {
		return nullptr;
	}

	return cameras[activeCamera];
}

void Scene::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

void Scene::setWindowColor(float r, float g, float b, float a)
{
	bg[0] = r;
	bg[1] = g;
	bg[2] = b;
	bg[3] = a;
}

void Scene::setWindowColor(glm::vec4 color)
{
	setWindowColor(color.r, color.g, color.b, color.a);
}
