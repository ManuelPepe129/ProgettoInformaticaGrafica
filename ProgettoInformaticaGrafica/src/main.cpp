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

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/model.h"
#include "graphics/light.h"
#include "graphics/text.h"

#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/models/box.hpp"
#include "graphics/models/sphere.hpp"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/camera.h"

#include "algorithms/states.hpp"

#include "scenes/menu.h"
#include "scenes/scene.h"

#include "entities/player.h"
#include "entities/enemy.h"

#include "physics/rigidbody.h"
#include "physics/environment.h"

struct Path {
	glm::vec3 start;
	glm::vec3 end;
};

//Joystick mainJ(0);

Camera cam;

Model axe("axe", BoundTypes::AABB, DYNAMIC);

void processInput(double dt, Scene* scene);

std::ostream& operator <<(std::ostream& out, const glm::vec3& v) {
	out << "[x: " << v.x << ", y: " << v.y
		<< ", z:" << v.z << "]";
	return out;
}

int main()
{
	Menu menu(3, 3, "Progetto Informatica Grafica", 800, 600);
	if (!menu.init())
	{
		std::cout << "Could not initialize Menu\n";
		return -1;
	}

	while (!menu.shouldClose() && !(menu.currentMenuState == MenuState::NEW_GAME))
	{
		// process input
		menu.processInput(0.0f);

		menu.render();
		menu.update();
		
		menu.newFrame();

	}

	if (!menu.shouldClose())
	{
		Scene scene(3, 3, "Progetto Informatica Grafica", 800, 600);
		

		//GameScene scene(3, 3, "Progetto Informatica Grafica", 800, 600);
		if (!scene.init()) {
			std::cout << "Could not init game scene" << std::endl;
			glfwTerminate();
			return -1;
		}

		scene.cameras.push_back(&cam);
		scene.activeCamera = 0;

		// SHADERS===============================
		Shader shader("assets/shaders/instanced_vs.glsl", "assets/shaders/core_fs.glsl");
		Shader lampShader("assets/shaders/instanced_vs.glsl", "assets/shaders/lamp_fs.glsl");
		Shader boxShader("assets/shaders/box_vs.glsl", "assets/shaders/box_fs.glsl");

		// MODELS

		Lamp lamp;
		scene.registerModel(&lamp);

		// TODO set maze material
		Model maze("maze", BoundTypes::AABB, NO_TEX | CONST_INSTANCES);
		maze.loadModel("assets/models/maze/maze.obj");
		scene.registerModel(&maze);
		scene.generateInstance(maze.id, glm::vec3(1.0f), 1.0f, glm::vec3(0.0f));
		//maze.setMaterial(Material::jade);

		/*
		Model exit("exit", BoundTypes::AABB, CONST_INSTANCES);
		exit.loadModel("assets/models/exit/scene.gltf");
		scene.registerModel(&exit);
		scene.generateInstance(exit.id, glm::vec3(1.0f), 1.0f, glm::vec3(0.0f));
		*/

		axe.loadModel("assets/models/axe/scene.gltf");
		scene.registerModel(&axe);

		Box box;
		box.init();

		Player* player = new Player(menu.getPlayerName(),&scene);
		player->setPlayerCamera(&cam);
		scene.addEntity(player);

		std::vector<Enemy*> enemies;

		std::vector<Path> enemyPaths =
		{
			Path{glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f)},
			Path{glm::vec3(-13.4f, 0.0f, 4.0f), glm::vec3(11.6f, 0.0f, 4.0f)},
			Path{glm::vec3(12.5f, 0.0f, -5.3f), glm::vec3(12.5f, 0.0f, 7.7f)},
			Path{glm::vec3(15.5f, 0.0f, -5.2f), glm::vec3(15.5f, 0.0f, 6.2f)},
			Path{glm::vec3(-12.2, 0.0f, -4.8f), glm::vec3(15.8f, 0.0f, -4.8f)},
			Path{glm::vec3(-26.0f, 0.0f, -.3f), glm::vec3(-26.0f, 0.0f, 14.9f)},
			Path{glm::vec3(-28.7f, 0.0f, 19.2f), glm::vec3(-4.0f, 0.0f, 19.2f)},
			Path{glm::vec3(24.5f, 0.0f, 16.4f), glm::vec3(18.0f, 0.0f, 16.4f)},
			Path{glm::vec3(-31.3f, 0.0f, 21.7f), glm::vec3(-2.0f, 0.0f, 21.41)},
			Path{glm::vec3(25.2f, 0.0f, 0.0f),glm::vec3(25.2f, 0.0f, 16.4f)},
			Path{glm::vec3(1.0f, 0.0f, 21.6f),glm::vec3(30.6f, 0.0f, 21.6f)},
			Path{glm::vec3(30.6f, 0.0f, 21.6f),glm::vec3(30.6f, 0.0f, 0.0f)},
			Path{glm::vec3(-30.8f, 0.0f, -23.2f),glm::vec3(-30.8f, 0.0f, -2.5f)},
			Path{glm::vec3(-30.8f, 0.0f, -2.5f),glm::vec3(-30.8f, 0.0f, 21.5f)}
		};

		Model enemyModel("monster", BoundTypes::AABB, DYNAMIC);
		enemyModel.loadModel("assets/models/monster/scene.glb");
		for (int i = 0; i < enemyPaths.size(); ++i)
		{
			Enemy* enemy = new Enemy(enemyModel.id, &scene);
			scene.registerModel(&enemyModel);
			enemy->init(glm::vec3(1.0f), 1.0f, enemyPaths[i].start);
			enemy->setPath(enemyPaths[i].start, enemyPaths[i].end);
			scene.addEntity(enemy);
			enemies.push_back(enemy);
		}

		scene.loadModels();

		// LIGHTS
		DirLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
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

		for (unsigned int i = 0; i < 4; i++) {
			pointLights[i] = PointLight(
				pointLightPositions[i],
				k0, k1, k2,
				ambient, diffuse, specular
			);
			scene.generateInstance(lamp.id, glm::vec3(0.25f), 0.25f, pointLightPositions[i]);
			scene.pointLights.push_back(&pointLights[i]);
			States::activate(&scene.activePointLights, i);
		}

		SpotLight spotLight(
			cam.cameraPos, cam.cameraFront,
			glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
			1.0f, 0.07f, 0.032f,
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f));
		scene.spotLights.push_back(&spotLight);
		scene.activeSpotLights = 1;	// 0b00000001

		scene.initInstances();

		scene.setBox(&box);

		double dt = 0.0f; // tme btwn frames
		double lastFrame = 0.0f; // time of last frame
		glfwSetTime(0.0f);
		while (!scene.shouldClose() && scene.state != GameState::GAME_OVER)
		{
			double currentTime = glfwGetTime();
			dt = currentTime - lastFrame;
			lastFrame = lastFrame + dt;

			scene.update(dt);

			// process input
			scene.processInput(dt);
			processInput(dt, &scene);

			//player.render(dt);
			//scene.render();

			scene.renderShader(shader);
			scene.renderInstances(maze.id, shader);
			//scene.renderShader(shader);
			//scene.renderInstances(exit.id, shader);

			scene.renderShader(shader);
			scene.renderInstances(enemyModel.id, shader);

			// remove launch objects if too far
			for (int i = 0; i < axe.currentNoInstances; i++) {
				if (glm::length(cam.cameraPos - axe.instances[i]->pos) > 100.0f) {
					scene.markForDeletion(axe.instances[i]->instanceId);
				}
			}

			if (axe.currentNoInstances > 0)
			{
				axe.update(dt);
				scene.renderShader(shader);
				scene.renderInstances(axe.id, shader);
			}

			scene.renderShader(lampShader, false);
			scene.renderInstances(lamp.id, lampShader);

			scene.renderShader(boxShader, false);
			box.render(boxShader);

			scene.renderText();

			// send new frame to window
			scene.newFrame();
			scene.clearDeadInstances();
		}
		menu.cleanup();
		scene.cleanup();
	}

	return 0;
}

void processInput(double dt, Scene* scene)
{
	if (Keyboard::keyWentDown(GLFW_KEY_P))
	{
		std::cout << cam.cameraPos <<std::endl;
	}
	if (Keyboard::keyWentDown(GLFW_KEY_1))
	{
		std::cout << "Launch axe\n";
		RigidBody* rb = scene->generateInstance(axe.id, glm::vec3(.50f), 1.0f, cam.cameraPos);
		if (rb) {
			// instance generated
			rb->transferEnergy(100.0f, cam.cameraFront);
			rb->applyAcceleration(Environment::gravitationalAcceleration);
		}
	}
}