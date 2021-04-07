#include "scene.h"

std::string Scene::generateId()
{
	for (int i = currentId.length() - 1; i >= 0; --i)
	{
		if ((int)currentId[i] != (int)'z')
		{
			currentId[i] = (char)(((int)currentId[i]) + 1);
			break;
		}
		else
		{
			currentId[i] = 'a';
		}
	}
	return currentId;
}

/*
	constructor
*/

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor,
	const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: BaseScene(glfwVersionMajor, glfwVersionMinor, title, scrWidth, scrHeight),
	activeCamera(-1),
	activePointLights(0), activeSpotLights(0),
	currentId("aaaaaaaa")
{ }

bool Scene::init()
{
	if (BaseScene::init()) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor
		octree = new Octree::Node(BoundingRegion(glm::vec3(-35.0f, -5.0f, -25.0f), glm::vec3(35.0f, 5.0f, 25.0f)));
		cameraBR = new BoundingRegion(cameraPos, 1.0f);
		return true;
	}
	return false;
}

void Scene::prepare(Box& box)
{
	octree->update(box);
}

/*
	main loop methods
*/

void Scene::newFrame(Box& box)
{
	// clear old positions and sizes
	box.positions.clear();
	box.sizes.clear();

	// process pending
	octree->processPending();
	octree->update(box);
	
	BaseScene::newFrame();
}

// process input
void Scene::processInput(float dt) {
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		setShouldClose(true);
	}
	if (Keyboard::keyWentDown(GLFW_KEY_P)) {
		glm::vec3 pos = cameraPos;
		std::cout << "[x: " << pos.x << ", y: " << pos.y
			<< ", z:" << pos.z << "]\n";
	}
	if (activeCamera != -1 && activeCamera < cameras.size()) {
		// active camera exists

		// set camera direction
		double mouseDX = Mouse::getDX();
		double mouseDY = Mouse::getDY();
		if ( mouseDX != 0.0 || mouseDY != 0)
		{
			cameras[activeCamera]->updateCameraDirection(mouseDX, mouseDY);
		}

		// set camera zoom
		//cameras[activeCamera]->updateCameraZoom(Mouse::getScrollDY());

		// set camera pos
		if (Keyboard::key(GLFW_KEY_W)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, dt);
			cameraBR->center = cameraPos;
			octree->checkCollisionsSelf(*cameraBR);
			octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_S)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, dt);
			cameraBR->center = cameraPos;
			octree->checkCollisionsSelf(*cameraBR);
			octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_D)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, dt);
			cameraBR->center = cameraPos;
			octree->checkCollisionsSelf(*cameraBR);
			octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_A)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, dt);
			cameraBR->center = cameraPos;
			octree->checkCollisionsSelf(*cameraBR);
			octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_SPACE)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::UP, dt);
			cameraBR->center = cameraPos;
			octree->checkCollisionsSelf(*cameraBR);
			octree->checkCollisionsChildren(*cameraBR);
		}
		if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
			cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, dt);
			cameraBR->center = cameraPos;
			octree->checkCollisionsSelf(*cameraBR);
			octree->checkCollisionsChildren(*cameraBR);
		}

		// set matrices
		view = cameras[activeCamera]->getViewMatrix();
		projection = glm::perspective(
			glm::radians(cameras[activeCamera]->getZoom()),	// FOV
			(float)scrWidth / (float)scrHeight,					// aspect ratio
			0.01f, 100.0f										// near and far bounds
		);

		// set pos at end
		cameraPos = cameras[activeCamera]->cameraPos;

		if (States::isIndexActive(&activeSpotLights, 0)) {
			spotLights[0]->position = cameraPos;
			spotLights[0]->direction = cameras[activeCamera]->cameraFront;
		}
	}
}

// set uniform shader varaibles (lighting, etc)
void Scene::renderShader(Shader shader, bool applyLighting) {
	// activate shader
	shader.activate();

	// set camera values
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.set3Float("viewPos", cameraPos);

	// lighting
	if (applyLighting) {
		// point lights
		unsigned int noLights = pointLights.size();
		unsigned int noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++) {
			if (States::isActive(&activePointLights, i)) {
				// i'th light is active
				pointLights[i]->render(shader, noActiveLights);
				noActiveLights++;
			}
		}
		shader.setInt("noPointLights", noActiveLights);

		// spot lights
		noLights = spotLights.size();
		noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++) {
			if (States::isActive(&activeSpotLights, i)) {
				// i'th spot light active
				spotLights[i]->render(shader, noActiveLights);
				noActiveLights++;
			}
		}
		shader.setInt("noSpotLights", noActiveLights);

		// directional light
		dirLight->render(shader);
	}
}

void Scene::renderInstances(std::string modelId, Shader shader, float dt)
{
	if (models[modelId])
	{
		models[modelId]->render(shader, dt, this);
	}
	else 
	{
		std::cout << "Cannot find model with " << modelId << " id\n";
	}
}

void Scene::render()
{
	std::cout << "Render function not defined without shader input" << std::endl;
}

void Scene::registerModel(Model* model)
{
	models.insert(model->id, model);
}

RigidBody* Scene::generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos)
{
	RigidBody* rb = models[modelId]->generateInstance(size, mass, pos);

	if (rb)
	{
		std::string id = generateId();
		rb->instanceId = id;
		instances.insert(id, rb);
		octree->addToPending(rb, models);
		return rb;
	}

	return nullptr;
}

void Scene::initInstances()
{
	models.traverse([](Model* model)->void {
		model->initInstances();
	});
}

void Scene::loadModels()
{
	models.traverse([](Model* model)->void {
		model->init();
	});
}

void Scene::removeInstance(std::string instanceId)
{
	/*
	*	remove all locations
	*	- Scene::instances
	*	- Model::instances
	*/

	std::string targetModel = instances[instanceId]->modelId;

	models[targetModel]->removeInstance(instanceId);

	instances[instanceId] = nullptr;

	instances.erase(instanceId);
}

void Scene::markForDeletion(std::string instanceId)
{
	States::activate(&instances[instanceId]->state, INSTANCE_DEAD);
	instancesToDelete.push_back(instances[instanceId]);
}

void Scene::clearDeadInstances()
{
	for (RigidBody* rb : instancesToDelete)
	{
		removeInstance(rb->instanceId);
	}
	instancesToDelete.clear();
}

void Scene::cleanup() {
	models.traverse([](Model* model)->void {
		model->cleanup();
	});

	octree->destroy();

	if (BaseScene::instances == 1) {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}

Camera* Scene::getActiveCamera() {
	return (activeCamera >= 0 && activeCamera < cameras.size()) ? cameras[activeCamera] : nullptr;
}

