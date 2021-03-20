#include "scene.h"

// TODO: Spostare tutta la logica della scena dal main a qua

/*
	constructor
*/
Scene::Scene() {}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor,
	const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: BaseScene(glfwVersionMajor, glfwVersionMinor, title, scrWidth, scrHeight),
	activeCamera(-1),
	activePointLights(0), activeSpotLights(0)
{ }

/*
	initialization
*/
/*
bool Scene::init() {
	glfwInit();

	// set version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	// initialize window
	window = glfwCreateWindow(scrWidth, scrHeight, title, NULL, NULL);
	if (window == NULL) {
		// not created
		return false;
	}
	glfwMakeContextCurrent(window);
	

	// set GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}

	// setup screen
	glViewport(0, 0, scrWidth, scrHeight);

		// callbacks

	// frame size
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	// key pressed
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	// cursor moved
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	// mouse btn pressed
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	// mouse scroll
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	// set rendering parameters

	glEnable(GL_DEPTH_TEST); // doesn't show vertices not visible to camera (back of object)

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor

	return true;
}
*/

/*
	main loop methods
*/
// process input
void Scene::processInput(float dt) {
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		setShouldClose(true);
	}
	if (activeCamera != -1 && activeCamera < cameras.size()) {
		// active camera exists

		// set camera direction
		cameras[activeCamera]->updateCameraDirection(Mouse::getDX(), 0);

		// set camera zoom
		//cameras[activeCamera]->updateCameraZoom(Mouse::getScrollDY());

		// set camera pos
		if (Keyboard::key(GLFW_KEY_W)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_S)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_D)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, dt);
		}
		if (Keyboard::key(GLFW_KEY_A)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, dt);
		}
		if (Keyboard::key(GLFW_KEY_SPACE)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::UP, dt);
		}
		if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, dt);
		}

		// set matrices
		view = cameras[activeCamera]->getViewMatrix();
		projection = glm::perspective(
			glm::radians(cameras[activeCamera]->getZoom()),	// FOV
			(float)scrWidth / (float)scrHeight,					// aspect ratio
			0.01f, 100.0f										// near and far bounds
		);

		// set pos at end
		cameraPos = cameras[activeCamera]->cameraPos;
	}
}

// set uniform shader varaibles (lighting, etc)
void Scene::render(Shader shader, bool applyLighting) {
	// activate shader
	shader.activate();

	// set camera values
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.set3Float("viewPos", cameraPos);

	// lighting
	if (applyLighting) {
		// point lights
		unsigned int noLights = pointLights.size();
		unsigned int noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++) {
			if (States::isActive(&activePointLights, i)) {
				// i'th light is active
				pointLights[i]->render(shader, noActiveLights);
				noActiveLights++;
			}
		}
		shader.setInt("noPointLights", noActiveLights);

		// spot lights
		noLights = spotLights.size();
		noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++) {
			if (States::isActive(&activeSpotLights, i)) {
				// i'th spot light active
				spotLights[i]->render(shader, noActiveLights);
				noActiveLights++;
			}
		}
		shader.setInt("noSpotLights", noActiveLights);

		// directional light
		dirLight->render(shader);
	}
}

void Scene::render()
{
	std::cout << "Render function not defined without shader input" << std::endl;
}

void Scene::cleanup() {
	if (window) {
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}

Camera* Scene::getActiveCamera() {
	return (activeCamera >= 0 && activeCamera < cameras.size()) ? cameras[activeCamera] : nullptr;
}

