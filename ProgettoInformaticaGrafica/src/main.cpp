#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/model.h"
#include "graphics/light.h"

#include "graphics/models/cube.h"
#include "graphics/models/lamp.h"
#include "graphics/models/sphere.h"
#include "graphics/models/box.h"

#include "physics/environment.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"
#include "io/camera.h"

#include "algorithms/states.hpp"

#include "scene.h"

Scene scene;

//Joystick mainJ(0);
void processInput(double deltaTime);

Camera camera(glm::vec3(-32.9f, 0.0f, -1.0f));

double deltaTime = 0.0f;
double lastTime = 0.0f;

int main()
{
	scene = Scene(3, 3, "Progetto Informatica Grafica", 800, 600);

	if (!scene.init())
	{
		std::cout << "Could not create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	scene.cameras.push_back(&camera);
	scene.activeCamera = 0;

	/* Shaders */

	Shader shader("assets/shaders/vertex_core.glsl", "assets/shaders/fragment_core.glsl");
	Shader lampShader("assets/shaders/vertex_core.glsl", "assets/shaders/fs_lamp.glsl");
	Shader boxShader("assets/shaders/vs_box.glsl", "assets/shaders/fs_box.glsl");

	/* Models */

	Box box;
	box.init();

	//Model m(BoundTypes::AABB, glm::vec3(0.0f), glm::vec3(1.0f), true);
	//m.loadModel("assets/models/Maze/Maze.obj");
	Model m(BoundTypes::AABB, glm::vec3(0.0f), glm::vec3(0.05f));
	m.loadModel("assets/models/lotr_troll/scene.gltf");
	//Model m(BoundTypes::AABB,glm::vec3(0.0f, -2.0f, -5.0f), glm::vec3(0.05f), true);
	//m.loadModel("assets/models/m4a1/scene.gltf");	

	DirectionalLight dirLight = {
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
		glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
	};
	scene.directionalLight = &dirLight;

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float c0 = 1.0f;
	float c1 = 0.09f;
	float c2 = 0.032f;

	PointLight pointLights[4];

	LampArray lamps;
	lamps.init();
	for (unsigned int i = 0; i < 4; ++i) 
	{
		pointLights[i] = {
			pointLightPositions[i],
			c0, c1, c2,
			ambient, diffuse, specular
		};
		lamps.lightInstances.push_back(pointLights[i]);
		scene.pointLights.push_back(&pointLights[i]);
		States::activate(&scene.activePointLights, i);
	}

	SpotLight spotLight = { 
		camera.getCameraPos(), 
		camera.getCameraFront(),
		glm::cos(glm::radians(12.5f)),  glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)
	};
	scene.spotLights.push_back(&spotLight);
	scene.activeSpotLights = 1;


	/*
	mainJ.update();

	if (mainJ.isPresent())
	{
		std::cout << mainJ.getName() << " is present" << std::endl;
	}
	*/

	double currentTime;

	// main loop
	while (!scene.shouldClose())
	{
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		box.positions.clear();
		box.sizes.clear();

		// process input
		processInput(deltaTime);

		// set background color
		scene.update();

		scene.render(shader);
		m.render(shader, deltaTime, &box);

		// lamps
		scene.render(lampShader, false);
		lamps.render(lampShader, deltaTime, &box);

		// render boxes
		if (box.positions.size() > 0) {
			// instances exist
			scene.render(boxShader, false);
			box.render(boxShader);
		}

		// send new frame to window
		scene.newFrame();
		
	}

	/* Clean Up */

	m.cleanup();
	box.cleanup();
	lamps.cleanup();

	scene.cleanup();
	return 0;
}

void processInput(double dt)
{
	scene.processInput(dt);

	// update flashlight
	if (States::isActive(&scene.activeSpotLights, 0))
	{
		scene.spotLights[0]->position = scene.getActiveCamera()->getCameraPos();
		scene.spotLights[0]->direction = scene.getActiveCamera()->getCameraFront();
	}

	// close window if escape or round button is pressed
	if (Keyboard::key(GLFW_KEY_ESCAPE))
	{
		scene.setShouldClose(true);
	}

	if (Keyboard::keyPressed(GLFW_KEY_L)) {
		States::toggle(&scene.activeSpotLights, 0); // toggle spot light
	}

	// TODO: Debug function - Needs to be removed
	if (Keyboard::keyPressed(GLFW_KEY_P))
	{
		glm::vec3 pos = camera.getCameraPos();
		std::cout << "x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
	}
}