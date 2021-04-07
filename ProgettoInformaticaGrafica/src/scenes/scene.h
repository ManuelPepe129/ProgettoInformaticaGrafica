#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "../graphics/light.h"
#include "../graphics/shader.h"
#include "../graphics/model.h"
#include "basescene.h"

#include "../io/camera.h"
#include "../io/keyboard.h"
#include "../io/mouse.h"

#include "../algorithms/states.hpp"
#include "../algorithms/trie.hpp"
#include "../algorithms/octree.h"

class Model;

class Scene : public BaseScene
{
public:
	trie::Trie<Model*> models;
	trie::Trie<RigidBody*> instances;

	std::vector<RigidBody*>instancesToDelete;

	Octree::Node* octree;

	/*
		constructor
	*/

	Scene(int glfwVersionMajor, int glfwVersionMinor,
		const char* title, unsigned int scrWidth, unsigned int scrHeight);

	/*
	*	initialization
	*/

	virtual bool init();

	// prepare for main loop (after object generation, etc)
	void prepare(Box& box);

	/*
		main loop methods
	*/

	virtual void newFrame(Box& box);

	// process input
	virtual void processInput(float dt);

	// set uniform shader varaibles (lighting, etc)
	virtual void renderShader(Shader shader, bool applyLighting = true);

	void renderInstances(std::string modelId, Shader shader, float dt);

	virtual void render();

	/*
	* Model / instance methods
	*/

	// add model to scene models array
	void registerModel(Model* model);

	// generate an instance based on modelId
	RigidBody* generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos);

	void initInstances();

	void loadModels();

	void removeInstance(std::string instanceId);

	void markForDeletion(std::string instanceId);
	void clearDeadInstances();

	std::string currentId;
	std::string generateId();

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

	BoundingRegion* cameraBR;

};
