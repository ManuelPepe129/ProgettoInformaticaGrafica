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
#include <glm/trigonometric.hpp>      // all the GLSL trigonometric functions: radians, cos, asin, etc.

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
#include <glm/gtx/norm.hpp>

struct Path {
	glm::vec3 start;
	glm::vec3 end;
};

//Joystick mainJ(0);

Camera cam;

void processInput(double dt, Player* scene);

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

		/* SHADERS */
		Shader shader("assets/shaders/instanced_vs.glsl", "assets/shaders/core_fs.glsl");
		Shader lampShader("assets/shaders/instanced_vs.glsl", "assets/shaders/lamp_fs.glsl");
		Shader boxShader("assets/shaders/box_vs.glsl", "assets/shaders/box_fs.glsl");

		/* MODELS */

		Lamp lamp;
		scene.registerModel(&lamp);

		Model maze("maze", BoundTypes::AABB, CONST_INSTANCES);
		maze.loadModel("assets/models/maze/scene.gltf");
		scene.registerModel(&maze);
		scene.generateInstance(maze.id, glm::vec3(1.0f), 1.0f, glm::vec3(0.0f));
		//maze.setMaterial(Material::grey);

		Model floor("floor", BoundTypes::AABB, CONST_INSTANCES);
		floor.loadModel("assets/models/floor/floor.obj");
		scene.registerModel(&floor);
		scene.generateInstance(floor.id, glm::vec3(1.0f), 1.0f, glm::vec3(0.0f, -1.f, 0.0f));

		Model book("book", BoundTypes::AABB, CONST_INSTANCES);
		book.loadModel("assets/models/book/books.obj");
		scene.registerModel(&book);
		scene.generateInstance(book.id, glm::vec3(1.0f), 1.0f, glm::vec3(10.5f, -1.0f, -2.7f));

		Model exit("exit", BoundTypes::AABB, CONST_INSTANCES);
		exit.loadModel("assets/models/exit/exit.obj");
		scene.registerModel(&exit);
		scene.generateInstance(exit.id, glm::vec3(1.0f), 1.0f, glm::vec3(-31.4f, .50f, -3.5f));

		Model couch("couch", BoundTypes::AABB, CONST_INSTANCES);
		couch.loadModel("assets/models/couch/couch.obj");
		//couch.loadModel("assets/models/exit/exit.obj");
		scene.registerModel(&couch);
		scene.generateInstance(couch.id, glm::vec3(.15f), 1.0f, glm::vec3(-10.0f, -.7f, -2.2f));

		Model candy("candy", BoundTypes::AABB, NO_TEX);
		candy.loadModel("assets/models/candy/candy.obj");
		candy.setMaterial(Material::cyan_plastic);
		scene.registerModel(&candy);
		scene.generateInstance(candy.id, glm::vec3(3.0f), 1.0f, glm::vec3(4.65f, 0.0f, 9.14f));
		scene.generateInstance(candy.id, glm::vec3(3.0f), 1.0f, glm::vec3(-21.78f, 0.0f, -7.31f));
		scene.generateInstance(candy.id, glm::vec3(3.0f), 1.0f, glm::vec3(12.49f, 0.f, -22.53f));
		scene.generateInstance(candy.id, glm::vec3(3.0f), 1.0f, glm::vec3(-21.0f, 0.f, -10.33f));

		Model painting("deChirico", BoundTypes::AABB, CONST_INSTANCES);
		painting.loadModel("assets/models/paintings/deChirico/DeChirico.obj");
		scene.registerModel(&painting);
		//scene.generateInstance(painting.id, glm::vec3(.20f), 1.0f, glm::vec3(0.0f), glm::vec3(glm::pi<float>()/2.0f,0.0f, glm::pi<float>()));
		States::activate(&scene.generateInstance(painting.id, glm::vec3(.20f), 1.0f, glm::vec3(20.0f, 0.2f, -1.7f), glm::vec3(glm::pi<float>() / 2.0f, 0.0f, glm::pi<float>()))->state, (unsigned char)NO_COLLISION);
		States::activate(&scene.generateInstance(painting.id, glm::vec3(.20f), 1.0f, glm::vec3(24.4f, 0.2f, -18.0f), glm::vec3(glm::pi<float>() / 2.0f, glm::pi<float>(), glm::pi<float>()))->state, (unsigned char)NO_COLLISION);
		States::activate(&scene.generateInstance(painting.id, glm::vec3(.20f), 1.0f, glm::vec3(-25.5f, 0.2f, -21.8), glm::vec3(glm::pi<float>() / 2.0f, 0.0f, glm::pi<float>()))->state, (unsigned char)NO_COLLISION);

		Model axe("axe", BoundTypes::AABB, DYNAMIC);
		axe.loadModel("assets/models/axe/axe.obj");
		scene.registerModel(&axe);

		Box box;
		box.init();

		Player* player = new Player(menu.getPlayerName(), &scene);
		player->setPlayerCamera(&cam);
		scene.addEntity(player);

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

		std::vector<Enemy*> enemies(enemyPaths.size());

		Model enemyModel("monster", BoundTypes::AABB, DYNAMIC | NO_TEX);
		enemyModel.loadModel("assets/models/monster/scene.glb");
		for (int i = 0; i < enemyPaths.size(); ++i)
		{
			Enemy* enemy = new Enemy(enemyModel.id, &scene);
			scene.registerModel(&enemyModel);
			enemy->init(glm::vec3(1.0f), 1.0f, enemyPaths[i].start);
			enemy->setPath(enemyPaths[i].start, enemyPaths[i].end);
			scene.addEntity(enemy);
			enemies[i] = enemy;
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
			processInput(dt, player);

			//player.render(dt);
			//scene.render();

			scene.renderShader(shader);
			scene.renderInstances(maze.id, shader);

			scene.renderShader(shader);
			scene.renderInstances(floor.id, shader);

			scene.renderShader(shader);
			scene.renderInstances(couch.id, shader);

			scene.renderShader(shader);
			scene.renderInstances(book.id, shader);

			scene.renderShader(shader);
			scene.renderInstances(candy.id, shader);

			scene.renderShader(shader);
			scene.renderInstances(exit.id, shader);

			scene.renderShader(shader);
			scene.renderInstances(painting.id, shader);

			scene.renderShader(shader);
			scene.renderInstances(enemyModel.id, shader);

			// remove launch objects if too far
			for (int i = 0; i < axe.currentNoInstances; i++) {
				if (axe.instances[i]->pos.y < -5.0f) {
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

			//scene.renderShader(boxShader, false);
			//box.render(boxShader);

			scene.renderText();

			// send new frame to window
			scene.newFrame();
			scene.clearDeadInstances();
		}

		delete player;

		for (unsigned int i = 0; i < enemies.size(); ++i)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
		enemies.clear();
	}

	return 0;
}

void processInput(double dt, Player* player)
{

	if (Keyboard::keyWentDown(GLFW_KEY_P))
	{
		std::cout << cam.cameraPos << std::endl;
	}

	if (Keyboard::keyWentDown(GLFW_KEY_1))
	{
		//std::cout << "Launch axe\n";
		/*
		if (scene)
		{
			if (scene->getAxes() > 0)
			{
				RigidBody* rb = scene->generateInstance(axe.id, glm::vec3(.50f), 1.0f, cam.cameraPos, glm::vec3(0.0, glm::radians(-cam.yaw + 90.0), 0.0));
				scene->removeAxe();
				if (rb) {
					// instance generated
					rb->transferEnergy(100.0f, cam.cameraFront);
					rb->applyAcceleration(Environment::gravitationalAcceleration);
				}
			}
			else
			{
				std::cout << "not enough axes\n";
			}

		}
		*/
		if (player)
		{
			player->throwAxe();
		}


	}

}