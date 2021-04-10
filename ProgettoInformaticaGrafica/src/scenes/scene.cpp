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

void Scene::setBox(Box* box)
{
	this->box = box;
}

/*
	constructor
*/

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor,
	const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: BaseScene(glfwVersionMajor, glfwVersionMinor, title, scrWidth, scrHeight),
	activeCamera(-1),
	activePointLights(0), activeSpotLights(0),
	currentId("aaaaaaaa"),
	points(0)
{ }

bool Scene::init()
{
	if (BaseScene::init()) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disable cursor
		//octree = new Octree::Node(BoundingRegion(glm::vec3(-35.0f, -5.0f, -25.0f), glm::vec3(35.0f, 5.0f, 25.0f)));
		cameraBR = new BoundingRegion(cameraPos, 0.3f);
		return true;
	}
	return false;
}

/*
void Scene::prepare(Box& box)
{
	//octree->update(box);
}
*/

/*
	main loop methods
*/

void Scene::newFrame()
{
	// clear old positions and sizes
	box->positions.clear();
	box->sizes.clear();

	// process pending
	//octree->processPending();
	//octree->update(box);

	BaseScene::newFrame();
}

void Scene::update(double dt)
{
	BaseScene::update();

	updateInstancies(dt);

	updateEntities(dt);

	cameraBR->center = cameras[activeCamera]->cameraPos;

	updateBoundings(dt);

	checkCollision(dt);
}

void Scene::checkCollision(double dt)
{
	for (int i = 0; i < objects.size(); ++i)
	{
		BoundingRegion br = objects[i];
		if (States::isActive(&br.instance->state, INSTANCE_MOVED) && !States::isActive(&instances[br.instance->instanceId]->state, INSTANCE_DEAD))
		{
			br.transform();
			if (br.instance->modelId == "axe")
			{
				for (int j = 0; j < objects.size(); ++j)
				{
					BoundingRegion other = objects[j];
					if (br.instance->instanceId != other.instance->instanceId && !States::isActive(&instances[other.instance->instanceId]->state, INSTANCE_DEAD))
					{
						other.transform();
						//std::cout << "Checking instance " << br.instance->instanceId << " of " << br.instance->modelId << " with instance" << other.instance->instanceId <<" of " << other.instance->modelId << std::endl;
						if (br.intersectsWith(other))
						{
							if (other.instance->modelId == "monster")
							{
								markForDeletion(other.instance->instanceId);
								points++;
							}
							std::cout << "Instance of model " << br.instance->modelId << " collides with instance of " << other.instance->modelId << std::endl;
							markForDeletion(br.instance->instanceId);
							break;
						}
					}
				}
				continue;
			}
		}
		else
		{
			if (br.intersectsWith(*cameraBR))
			{
				//std::cout << "Collision with camera" << std::endl;
				cameras[activeCamera]->reverseCameraPos(dt);
			}
		}

	}
	//std::cout << "-----" << std::endl;
}

// process input
void Scene::processInput(float dt)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE))
	{
		setShouldClose(true);
	}
	if (activeCamera != -1 && activeCamera < cameras.size())
	{

		// set matrices
		view = cameras[activeCamera]->getViewMatrix();
		projection = glm::perspective(
			glm::radians(cameras[activeCamera]->getZoom()),	// FOV
			(float)scrWidth / (float)scrHeight,					// aspect ratio
			0.01f, 100.0f										// near and far bounds
		);

		// set pos at end
		cameraPos = cameras[activeCamera]->cameraPos;

		if (States::isIndexActive(&activeSpotLights, 0))
		{
			spotLights[0]->position = cameraPos;
			spotLights[0]->direction = cameras[activeCamera]->cameraFront;
		}
	}
}

// set uniform shader varaibles (lighting, etc)
void Scene::renderShader(Shader shader, bool applyLighting)
{
	// activate shader
	shader.activate();

	// set camera values
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.set3Float("viewPos", cameraPos);

	// lighting
	if (applyLighting)
	{
		// point lights
		unsigned int noLights = pointLights.size();
		unsigned int noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++)
		{
			if (States::isActive(&activePointLights, i))
			{
				// i'th light is active
				pointLights[i]->render(shader, noActiveLights);
				noActiveLights++;
			}
		}
		shader.setInt("noPointLights", noActiveLights);

		// spot lights
		noLights = spotLights.size();
		noActiveLights = 0;
		for (unsigned int i = 0; i < noLights; i++)
		{
			if (States::isActive(&activeSpotLights, i))
			{
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

void Scene::renderInstances(std::string modelId, Shader shader)
{
	if (models[modelId])
	{
		models[modelId]->render(shader, this);
	}
	else
	{
		std::cerr << "Cannot find model with " << modelId << " id\n";
	}
}

void Scene::render()
{
	std::cerr << "Render function not defined without shader input\n";
}

void Scene::addEntity(EntityBase* entity)
{
	entities.push_back(entity);
}

void Scene::registerModel(Model* model)
{
	if (models.count(model->id) == 0)
	{
		//std::cout << "Registering model " << model->id << std::endl;
		models.insert(std::pair<std::string, Model*>(model->id, model));
	}

}

RigidBody* Scene::generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos)
{
	RigidBody* rb = models[modelId]->generateInstance(size, mass, pos);

	if (rb)
	{
		std::string id = generateId();
		rb->instanceId = id;
		instances.insert(id, rb);
		addToPending(rb);
		return rb;
	}

	return nullptr;
}
void Scene::initInstances()
{
	for (std::map<std::string, Model*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		Model* model = it->second;
		model->initInstances();
	}
}

void Scene::loadModels()
{
	for (std::map<std::string, Model*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		Model* model = it->second;
		model->init();
	}
}

void Scene::removeInstance(std::string instanceId)
{
	// remove all locations
	//	- Scene::instances
	//	- Model::instances

	std::string targetModel = instances[instanceId]->modelId;

	models[targetModel]->removeInstance(instanceId);

	instances[instanceId] = nullptr;

	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].instance->instanceId == instanceId)
		{
			std::cout << "deleting br " << objects[i].instance->instanceId << std::endl;
			for (int j = 0; j < models[targetModel]->boundingRegions.size(); ++j)
			{
				objects.erase(objects.begin() + i);
			}

			break;
		}
	}

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
		std::cout << "Deleting " << rb->instanceId << " of model " << rb->modelId << '\n';
		removeInstance(rb->instanceId);
	}
	instancesToDelete.clear();
}

void Scene::addToPending(RigidBody* instance)
{
	for (BoundingRegion br : models[instance->modelId]->boundingRegions)
	{
		br.instance = instance;
		br.transform();
		objects.push_back(br);
	}
}

const int Scene::getPoints() const
{
	return points;
}

void Scene::cleanup() {
	for (std::map<std::string, Model*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		Model* model = it->second;
		model->cleanup();
	}

	//octree->destroy();

	if (BaseScene::instances == 1)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}

Camera* Scene::getActiveCamera()
{
	return (activeCamera >= 0 && activeCamera < cameras.size()) ? cameras[activeCamera] : nullptr;
}

void Scene::updateEntities(double dt)
{
	for (int i = 0; i < entities.size(); ++i)
	{
		entities[i]->update(dt);
	}
}

void Scene::updateBoundings(double dt)
{
	for (BoundingRegion br : objects)
	{
		if (States::isActive(&br.instance->state, INSTANCE_MOVED))
		{
			br.transform();
			//std::cout << br.instance->modelId << std::endl;
		}
		box->positions.push_back(br.calculateCenter());
		box->sizes.push_back(br.calculateDimensions());
	}
}

void Scene::updateInstancies(double dt)
{
	for (std::map<std::string, Model*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		Model* model = it->second;
		if (model->currentNoInstances > 0)
		{
			model->update(dt);
		}
	}
}

