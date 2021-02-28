#include "screen.h"

#include "keyboard.h"
#include "mouse.h"

unsigned int Screen::SCR_WIDTH = 800;
unsigned int Screen::SCR_HEIGHT = 600;

void Screen::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

Screen::Screen():
	window(nullptr)
{
}

bool Screen::init()
{
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Progetto Informatica Grafica", NULL, NULL);
	if (!window)
	{
		return false;
	}
	glfwMakeContextCurrent(window);
}

void Screen::setParameters()
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glfwSetFramebufferSizeCallback(window, Screen::framebufferSizeCallback);

	glfwSetKeyCallback(window, Keyboard::keyCallback);

	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);

	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);

	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // removes cursor from viewport

	glEnable(GL_DEPTH_TEST);

}

void Screen::update()
{
	// set background color
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::newFrame()
{
	// send new frame to window
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Screen::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Screen::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

void Screen::setBackgroundColor(glm::vec3 background)
{
	backgroundColor = background;
}
