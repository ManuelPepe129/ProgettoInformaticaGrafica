#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>

#include "graphics/light.h"
#include "graphics/shader.h"
#include "graphics/basescene.h"

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

#include "algorithms/states.hpp"


class Scene : public BaseScene
{
public:
	/*
		constructor
	*/
	Scene();
	Scene(int glfwVersionMajor, int glfwVersionMinor,
		const char* title, unsigned int scrWidth, unsigned int scrHeight);

	/*
		main loop methods
	*/

	// process input
	virtual void processInput(float dt);

	// set uniform shader varaibles (lighting, etc)
	virtual void render(Shader shader, bool applyLighting = true);

	virtual void render();

	/*
		cleanup method
	*/
	virtual void cleanup();

	Camera* getActiveCamera();

	/*
		lights
	*/

	// list of point lights
	std::vector<PointLight*> pointLights;
	unsigned int activePointLights;
	// list of spot lights
	std::vector<SpotLight*> spotLights;
	unsigned int activeSpotLights;
	// direction light
	DirLight* dirLight;
	bool dirLightActive;

	/*
		camera
	*/
	std::vector<Camera*> cameras;
	unsigned int activeCamera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cameraPos;

};

#endif