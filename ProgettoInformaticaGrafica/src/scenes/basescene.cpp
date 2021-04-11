#include "basescene.h"
#include <iostream>

unsigned int BaseScene::scrWidth = 0;
unsigned int BaseScene::scrHeight = 0;
unsigned int BaseScene::instances = 0;
GLFWwindow* BaseScene::window = nullptr;

std::string BaseScene::playerName = "";

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void BaseScene::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	BaseScene::scrWidth = width;
	BaseScene::scrHeight = height;
}

BaseScene::BaseScene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight, SceneType sceneType)
	: glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor), title(title), sceneType(sceneType) {

	BaseScene::scrWidth = scrWidth;
	BaseScene::scrHeight = scrHeight;

	setBackgroundColor(0.1f, 0.15f, 0.15f, 1.0f);

	BaseScene::instances++;
}

BaseScene::~BaseScene()
{
	BaseScene::instances--;
	if (instances == 1)
	{
		//delete window;
	}
}

bool BaseScene::init()
{
	if (!BaseScene::window)
	{
		std::cout << "init window" << std::endl;
		glfwSetErrorCallback(glfw_error_callback);

		// initialize GLFW and window
		if (!glfwInit())
		{
			return false;
		}

		// set version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// initialize window
		BaseScene::window = glfwCreateWindow(scrWidth, scrHeight, title, NULL, NULL);
		if (BaseScene::window == NULL) {
			// not created
			return false;
		}
		glfwMakeContextCurrent(BaseScene::window);


		// set GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			glfwTerminate();
			return false;
		}

		// setup screen
		glViewport(0, 0, scrWidth, scrHeight);

		/*
			callbacks
		*/
		// frame size
		glfwSetFramebufferSizeCallback(BaseScene::window, framebufferSizeCallback);
		// key pressed
		glfwSetKeyCallback(BaseScene::window, Keyboard::keyCallback);
		// cursor moved
		glfwSetCursorPosCallback(BaseScene::window, Mouse::cursorPosCallback);
		// mouse btn pressed
		glfwSetMouseButtonCallback(BaseScene::window, Mouse::mouseButtonCallback);
		// mouse scroll
		glfwSetScrollCallback(BaseScene::window, Mouse::mouseWheelCallback);

		/*
			set rendering parameters
		*/

		glEnable(GL_DEPTH_TEST ); // doesn't show vertices not visible to camera (back of object)

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}
	return true;
}

void BaseScene::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(BaseScene::window, shouldClose);
}

void BaseScene::setBackgroundColor(float r, float g, float b, float a)
{
	background = glm::vec4(r, g, b, a);
}

void BaseScene::setBackgroundColor(glm::vec4 background)
{
	this->background = background;
}

GLFWwindow* BaseScene::getWindow()
{
	return window;
}

const std::string BaseScene::getPlayerName() const
{
	return playerName;
}

void BaseScene::processInput(float dt)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		setShouldClose(true);
	}
}

// update screen before each frame
void BaseScene::update() {
	glClearColor(background.x, background.y, background.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// update screen after frame
void BaseScene::newFrame() {
	// send new frame to window
	glfwSwapBuffers(window);
	glfwPollEvents();
}

