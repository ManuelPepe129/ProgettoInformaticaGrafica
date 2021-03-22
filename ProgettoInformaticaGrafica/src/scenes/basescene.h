#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>

#include "../io/camera.h"
#include "../io/keyboard.h"
#include "../io/mouse.h"

enum class SceneType {
	MENU,
	GAME
};

class BaseScene
{
public:
	/*
		callbacks
	*/
	// window resize
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	BaseScene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight, SceneType sceneType=SceneType::GAME);

	~BaseScene();

	bool init();

	virtual void render()=0;

	virtual void cleanup()=0;

	virtual void processInput(float dt);

	// update screen before each frame
	void update();

	// update screen after frame
	void newFrame();

	bool shouldClose() {
		return glfwWindowShouldClose(window);
	}

	/*
		modifiers
	*/
	void setShouldClose(bool shouldClose);

	void setBackgroundColor(float r, float g, float b, float a=1.0f);
	void setBackgroundColor(glm::vec4 background);

	GLFWwindow* getWindow();

	SceneType sceneType;

protected:
	// window object
	static GLFWwindow* window;

	glm::vec4 background;

	// window vals
	const char* title;
	static unsigned int scrWidth;
	static unsigned int scrHeight;

	// GLFW info
	int glfwVersionMajor;
	int glfwVersionMinor;

	static unsigned int instances;
};
