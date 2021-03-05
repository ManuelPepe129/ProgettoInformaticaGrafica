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
#include "io/screen.h"
#include "io/camera.h"

// Callbacks
void processInput(double dt);

glm::mat4 mouseTransform = glm::mat4(1.0f);
Joystick mainJ(0);
glm::mat4 transform = glm::mat4(1.0f);

Camera camera(glm::vec3(-32.9f, 0.0f, -1.0f));

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

# ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COPMPAT, GL_TRUE);
#endif

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

	Shader shader("assets/shaders/vertex_core.glsl", "assets/shaders/fragment_core.glsl");
	Shader lampShader("assets/shaders/vertex_core.glsl", "assets/shaders/fs_lamp.glsl");
	Shader boxShader("assets/shaders/vs_box.glsl", "assets/shaders/fs_box.glsl");

	/* Models */

	Box box;
	box.init();

	Model m(BoundTypes::AABB,glm::vec3(0.0f), glm::vec3(1.0f), true);
	m.loadModel("assets/models/Maze/Maze.obj");
	//Model m1(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.05f));
	//m1.loadModel("assets/models/lotr_troll/scene.gltf");
	//Model m(BoundTypes::AABB,glm::vec3(0.0f, -2.0f, -5.0f), glm::vec3(0.05f), true);
	//m.loadModel("assets/models/m4a1/scene.gltf");	

	DirectionalLight dirLight = {
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
		glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
	};

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

	LampArray lamps;
	lamps.init();
	for (unsigned int i = 0; i < 4; i++) 
	{
		lamps.lightInstances.push_back({
			pointLightPositions[i],
			c0, c1, c2,
			ambient, diffuse, specular
			});
	}

	SpotLight spotLight = { 
		camera.getCameraPos(), 
		camera.getCameraFront(),
		glm::cos(glm::radians(12.5f)),  glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
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

		box.positions.clear();
		box.sizes.clear();

		// process input
		processInput(deltaTime);

		// set background color
		screen.update();

		shader.activate();
		shader.set3Float("viewPosition", camera.getCameraPos());

		dirLight.render(shader);

		for (unsigned int i = 0; i < 4; i++) {
			shader.activate();
			lamps.lightInstances[i].render(shader, i);
		}
		shader.setInt("noPointLights", 4);

		spotLight.position = camera.getCameraPos();
		spotLight.direction = camera.getCameraFront();
		spotLight.render(shader, 0);
		shader.setInt("noSpotLights", 1);

		// create transformation for screen
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getCameraZoom()), (float)Screen::SCR_WIDTH / (float)Screen::SCR_HEIGHT, 0.1f, 100.0f);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		m.render(shader, deltaTime, &box);

		// lamps
		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		lamps.render(lampShader, deltaTime, &box);

		// render boxes
		if (box.positions.size() > 0) {
			// instances exist
			boxShader.activate();
			boxShader.setMat4("view", view);
			boxShader.setMat4("projection", projection);
			box.render(boxShader);
		}

		// send new frame to window
		screen.newFrame();
		
	}

	/* Clean Up */

	//m1.cleanup();
	m.cleanup();
	box.cleanup();
	lamps.cleanup();

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

	if (Keyboard::key(GLFW_KEY_P))
	{
		glm::vec3 pos = camera.getCameraPos();
		std::cout << "x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
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