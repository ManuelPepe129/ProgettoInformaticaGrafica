#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

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

	virtual ~BaseScene();

	virtual bool init();

	virtual void render()=0;

	virtual void cleanup()=0;

	virtual void processInput(float dt);

	// update screen before each frame
	virtual void update();

	// update screen after frame
	virtual void newFrame();

	virtual bool shouldClose() {
		return glfwWindowShouldClose(window);
	}

	/*
		modifiers
	*/
	void setShouldClose(bool shouldClose);

	void setBackgroundColor(float r, float g, float b, float a=1.0f);
	void setBackgroundColor(glm::vec4 background);

	/*
	*	Getters
	*/

	GLFWwindow* getWindow();

	virtual const std::string getPlayerName() const;

	SceneType sceneType;

protected:
	// window object
	static GLFWwindow* window;

	static std::string playerName;

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

