#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "graphics/light.h"
#include "graphics/shader.h"

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

#include "algorithms/states.hpp"

#include <vector>

class Scene
{
public:
	/*
	*	Callbacks
	*/

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	/*
	*	Consructors
	*/

	Scene() {};
	Scene(int glfwVersionMajor, int glfWVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight);

	/*
	*	Initializations
	*/

	bool init();

	/*
	*	Main loop methods
	*/

	// process input
	void processInput(float dt);

	// update screen before each frame
	void update();

	//update screen after each frame
	void newFrame();

	// set uniform shader variables (lighting, etc...)
	void render(Shader shader, bool applyLighting = true);

	/*
	*	Cleanup
	*/
	void cleanup();

	/*
	*	Getters & Setters
	*/

	bool shouldClose();

	Camera* getActiveCamera();

	void setShouldClose(bool shouldClose);

	void setWindowColor(float r, float g, float b, float a = 1.0f);
	void setWindowColor(glm::vec4 color);

	/*
	*	Lights
	*/

	// list of point lights
	std::vector<PointLight*> pointLights;
	unsigned int activePointLights;

	// list of spot lights
	std::vector<SpotLight*> spotLights;
	unsigned int activeSpotLights;

	// directional light
	DirectionalLight* directionalLight;
	bool directionalLightActive;

	/*
	*	Cameras
	*/
	std::vector<Camera*> cameras;
	unsigned int activeCamera; // can be zero: es. in menu
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cameraPos;

protected:
	// window object
	GLFWwindow* window;

	const char* title;
	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	float bg[4] = { 0.2f, 0.3f, 0.3f, 1.0f}; // background color

	// GLFW
	int glfwVersionMajor;
	int glfWVersionMinor;
};

