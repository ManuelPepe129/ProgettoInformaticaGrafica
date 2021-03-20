#include "gamescene.h"

GameScene::GameScene(int glfwVersionMajor, int glfwVersionMinor,
	const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: Scene(glfwVersionMajor, glfwVersionMinor,
		title, scrWidth, scrHeight)
{

}

bool GameScene::init()
{
	if (Scene::init())
	{
		Camera cam;
		cameras.push_back(&cam);
		activeCamera = 0;

		// SHADERS===============================
		shader = Shader("assets/shaders/core_vs.glsl", "assets/shaders/core_fs.glsl");
		lampShader = Shader("assets/shaders/instanced_vs.glsl", "assets/shaders/lamp_fs.glsl");
		boxShader = Shader("assets/shaders/box_vs.glsl", "assets/shaders/box_fs.glsl");

		// MODELS==============================

		box.init();

		Model m(BoundTypes::AABB, glm::vec3(0.0f), glm::vec3(1.0f), true);
		m.loadModel("assets/models/maze/maze.obj");
		models.push_back(m);

		// LIGHTS
		dirLight = new DirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

		glm::vec3 pointLightPositions[] =
		{
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

		PointLight pointLights_tmp[4];

		lamps.init();
		for (unsigned int i = 0; i < 4; i++)
		{
			pointLights_tmp[i] = {
				pointLightPositions[i],
				k0, k1, k2,
				ambient, diffuse, specular
			};
			lamps.lightInstances.push_back(pointLights_tmp[i]);
			pointLights.push_back(&pointLights_tmp[i]);
			States::activate(&activePointLights, i);
		}

		spotLights.push_back(new SpotLight(cam.cameraPos, cam.cameraFront,
			glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
			1.0f, 0.07f, 0.032f,
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)));

		activeSpotLights = 1;	// 0b00000001

		return true;
	}
	return false;
}

void GameScene::render()
{
	box.positions.clear();
	box.sizes.clear();

	Scene::render(shader);
	models[0].render(shader, &box);

	// lamps
	Scene::render(lampShader, false);
	lamps.render(lampShader, &box);

	// render boxes
	if (box.positions.size() > 0) {
		// instances exist
		Scene::render(boxShader, false);
		box.render(boxShader);
	}

}

void GameScene::cleanup()
{
	// clean up objects
	lamps.cleanup();
	box.cleanup();
	for (int i = 0; i < models.size(); i++)
	{
		models[i].cleanup();
	}
	Scene::cleanup();
}

