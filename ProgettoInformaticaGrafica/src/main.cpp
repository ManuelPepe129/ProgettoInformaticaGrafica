#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>

#include <iostream>
#include <string>

#include "graphics/Shader.h"
#include "graphics/texture.h"
#include "graphics/material.h"

#include "graphics/models/cube.h"
#include "graphics/models/lamp.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"
#include "io/camera.h"
#include "io/screen.h"

// Callbacks
void processInput(double dt);

glm::mat4 mouseTransform = glm::mat4(1.0f);
Joystick mainJ(0);
glm::mat4 transform = glm::mat4(1.0f);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime = 0.0f;
float lastTime = 0.0f;

Screen screen;

int main()
{
	glfwInit();

	// OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Use OpenGL Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!screen.init())
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	screen.setParameters();
	glEnable(GL_DEPTH_TEST);

	/* Shaders */

	Shader shader("assets/vertex_core.glsl", "assets/fragment_core.glsl");
	Shader lampShader("assets/vertex_core.glsl", "assets/fs_lamp.glsl");

	/* Geometry */

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	Cube cubes[10];
	for (unsigned int i = 0; i < 10; i++) {
		cubes[i] = Cube(cubePositions[i], glm::vec3(1.0f),Material::gold);
		cubes[i].init();
	}

	glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};
	Lamp lamps[4];
	for (unsigned int i = 0; i < 4; i++) {
		lamps[i] = Lamp(pointLightPositions[i], glm::vec3(0.25f), 
			glm::vec3(1.0f),
			1.0f, 0.07f, 0.032f, 
			glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f));
		lamps[i].init();
	}

	DirectionalLight dirLight = { glm::vec3(-0.2f, -1.0f, -0.3f),1.0f, 0.07f, 0.032f, glm::vec3(0.1f), glm::vec3(0.4f), glm::vec3(0.5f) };

	SpotLight spotLight = { 
		camera.getCameraPos(), 
		camera.getCameraFront(),
		glm::cos(glm::radians(12.5f)),  glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec3(0.f), glm::vec3(1.0f), glm::vec3(1.0f)
	};

	mainJ.update();

	if (mainJ.isPresent())
	{
		std::cout << mainJ.getName() << " is present" << std::endl;
	}
	else
	{
		std::cout << "Joystick not present" << std::endl;
	}

	double currentTime;

	// main loop
	while (!screen.shouldClose())
	{
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// process input
		processInput(deltaTime);

		/*
			Render
		*/

		// set background color
		screen.update();

		shader.activate();
		shader.set3Float("viewPosition", camera.getCameraPos());

		dirLight.direction = glm::vec3(
			glm::rotate(glm::mat4(1.0f), glm::radians(.5f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::vec4(dirLight.direction,1.0f)
		);
		
		dirLight.render(shader);

		for (int i = 0; i < 4; i++)
		{
			lamps[i].pointLight.render(shader,i);
		}
		shader.setInt("noPointLights", 4);

		spotLight.position = camera.getCameraPos();
		spotLight.direction = camera.getCameraFront();
		spotLight.render(shader,0);
		shader.setInt("noSpotLights", 0);

		// create transformation for screen
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getCameraZoom()), (float)Screen::SCR_WIDTH / (float)Screen::SCR_HEIGHT, 0.1f, 100.0f);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		for (int i = 0; i < 10; i++) {
			cubes[i].render(shader);
		}
		

		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		for (int i = 0; i < 4; i++)
		{
			lamps[i].render(lampShader);
		}


		// send new frame to window
		screen.newFrame();
		
	}

	/* Clean Up */
	for (int i = 0; i < 10; i++) {
		cubes[i].cleanup();
	}
	for (int i = 0; i < 4; i++)
	{
		lamps[i].cleanup();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();

	return 0;
}

void processInput(double dt)
{
	// close window if escape or round button is pressed
	if (Keyboard::key(GLFW_KEY_ESCAPE) || mainJ.buttonState(GLFW_JOYSTICK_BTN_RIGHT))
	{
		screen.setShouldClose(true);
	}

	/* move camera */

	if (Keyboard::key(GLFW_KEY_W))
	{
		camera.updateCameraPos(CameraDirection::FORWARD, dt);
	}

	if (Keyboard::key(GLFW_KEY_S))
	{
		camera.updateCameraPos(CameraDirection::BACKWARD, dt);
	}

	if (Keyboard::key(GLFW_KEY_A))
	{
		camera.updateCameraPos(CameraDirection::LEFT, dt);
	}

	if (Keyboard::key(GLFW_KEY_D))
	{
		camera.updateCameraPos(CameraDirection::RIGHT, dt);
	}

	if (Keyboard::key(GLFW_KEY_SPACE))
	{
		camera.updateCameraPos(CameraDirection::UP, dt);
	}

	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT))
	{
		camera.updateCameraPos(CameraDirection::DOWN, dt);
	}

	double dx = Mouse::getDX(), dy = Mouse::getDY();
	if (dx != 0 || dy != 0) {
		camera.updateCameraDirection(dx, dy);
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