/*
	CODE
	ENGINE (Scene)
	OPENGL
	GPU
*/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/model.h"
#include "graphics/light.h"
#include "graphics/text.h"
#include "graphics/menu.h"

#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"
#include "graphics/models/box.hpp"

#include "physics/environment.h"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/joystick.h"
#include "io/camera.h"

#include "algorithms/states.hpp"

#include "scene.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Scene scene;

void processInput(double dt);

Camera cam;

//Joystick mainJ(0);

double dt = 0.0f; // tme btwn frames
double lastFrame = 0.0f; // time of last frame

unsigned int VAO, VBO;

std::vector<Model> models;

int main() {

	scene = Scene(3, 3, "Progetto Informatica Grafica", 800, 600);

	if (!scene.init()) {
		std::cout << "Could not open window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Menu menu(3, 3, "Progetto Informatica Grafica", 800, 600);
	//menu.init();

	/*
	*	Text Rendering Library
	*/

	TextRenderer textRenderer("assets/fonts/comic.ttf", 48);
	//textRenderer.init();

	scene.cameras.push_back(&cam);
	scene.activeCamera = 0;

	// SHADERS===============================
	Shader shader("assets/shaders/core_vs.glsl", "assets/shaders/core_fs.glsl");
	Shader lampShader("assets/shaders/instanced_vs.glsl", "assets/shaders/lamp_fs.glsl");
	Shader boxShader("assets/shaders/box_vs.glsl", "assets/shaders/box_fs.glsl");
	Shader textShader("assets/shaders/glyph_vs.glsl", "assets/shaders/glyph_fs.glsl");

	// MODELS==============================

	Box box;
	box.init();

	Model m(BoundTypes::AABB, glm::vec3(0.0f), glm::vec3(1.0f), true);
	m.loadModel("assets/models/maze/maze.obj");
	models.push_back(m);
	//m.loadModel("assets/models/cube/cube.obj");

	// LIGHTS
	DirLight dirLight = { glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f) };
	scene.dirLight = &dirLight;

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float k0 = 1.0f;
	float k1 = 0.09f;
	float k2 = 0.032f;

	PointLight pointLights[4];

	LampArray lamps;
	lamps.init();
	for (unsigned int i = 0; i < 4; i++) {
		pointLights[i] = {
			pointLightPositions[i],
			k0, k1, k2,
			ambient, diffuse, specular
		};
		lamps.lightInstances.push_back(pointLights[i]);
		scene.pointLights.push_back(&pointLights[i]);
		States::activate(&scene.activePointLights, i);
	}

	SpotLight spotLight = {
		cam.cameraPos, cam.cameraFront,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
		1.0f, 0.07f, 0.032f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)
	};
	scene.spotLights.push_back(&spotLight);
	scene.activeSpotLights = 1;	// 0b00000001

	// joystick recognition
	/*mainJ.update();
	if (mainJ.isPresent()) {
		std::cout << mainJ.getName() << " is present." << std::endl;
	}*/

	while (!scene.shouldClose()) {
		// calculate dt
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		// process input
		processInput(dt);

		//menu.render();
		//menu.update();
		//menu.newFrame();


		box.positions.clear();
		box.sizes.clear();

		// process input
		processInput(dt);

		Camera* camera = scene.getActiveCamera();
		for (unsigned int i = 0; i < models.size(); ++i) {
			m = models[i];
			for (unsigned int j = 0; j < m.meshes.size(); ++j)
			{
				BoundingRegion tmp = m.meshes[j].br;
				if (tmp.containsPoint(camera->cameraPos))
				{
					switch (m.type) {
					case ModelType::WALL:
						scene.getActiveCamera()->updateCameraPos(camera->lastDirection, 2 * dt);
						break;
					case ModelType::COLLECTABLE:
						m.toBeDestroyed = true;
						break;
					}
					break; // max 1 collisione gestita
				}
			}
		}

		// update screen values
		scene.update();

		scene.render(shader);
		m.render(shader, dt, &box);

		// lamps
		scene.render(lampShader, false);
		lamps.render(lampShader, dt, &box);

		// render boxes
		if (box.positions.size() > 0) {
			// instances exist
			scene.render(boxShader, false);
			box.render(boxShader);
		}

		textRenderer.render(textShader, std::to_string((int)currentTime), 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		// send new frame to window
		scene.newFrame();

	}

	// clean up objects
	lamps.cleanup();
	box.cleanup();
	m.cleanup();

	scene.cleanup();
	//menu.cleanup();
	return 0;
}

void processInput(double dt) {
	scene.processInput(dt);

	// update flash light
	if (States::isActive(&scene.activeSpotLights, 0)) {
		scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
		scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
	}

	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		scene.setShouldClose(true);
	}

	if (Keyboard::keyWentDown(GLFW_KEY_L)) {
		States::toggle(&scene.activeSpotLights, 0); // toggle spot light
	}
}