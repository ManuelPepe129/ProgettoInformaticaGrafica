#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Screen
{
public:
	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	Screen();

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	bool init();

	// sets viewport and callbacks
	void setParameters();

	// main loop
	void update();
	void newFrame();

	// window closing accessor and modifier
	bool shouldClose();

	void setShouldClose(bool shouldClose);

	void setBackgroundColor(glm::vec3 background);

private:

	GLFWwindow* window;

	glm::vec3 backgroundColor = glm::vec3(0.2f, 0.3f, 0.3f);
};

