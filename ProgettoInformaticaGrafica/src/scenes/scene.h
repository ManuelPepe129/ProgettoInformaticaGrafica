#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>

#include "basescene.h"

#include "../graphics/light.h"
#include "../graphics/shader.h"
#include "../graphics/model.h"

#include "../io/camera.h"
#include "../io/keyboard.h"
#include "../io/mouse.h"

#include "../algorithms/states.hpp"
#include "../algorithms/trie.hpp"

#include "../entities/entitybase.h"
#include "../physics/rigidbody.h"
#include <queue>
#include "../graphics/text.h"

class Model;
class EntityBase;

enum class GameState
{
	GAME_OVER,
	PLAYING
};

class Scene : public BaseScene
{
public:
	std::map<std::string, Model*> models;
	trie::Trie<RigidBody*> instances;
	std::vector<EntityBase*> entities;

	std::vector<RigidBody*>instancesToDelete;

	GameState state;

	//Octree::Node* octree;

	/*
		constructor
	*/

	Scene(int glfwVersionMajor, int glfwVersionMinor,
		const char* title, unsigned int scrWidth, unsigned int scrHeight);

	~Scene();

	/*
	*	initialization
	*/

	virtual bool init();

	// prepare for main loop (after object generation, etc)
	//void prepare(Box& box);

	/*
		main loop methods
	*/

	virtual void newFrame();

	virtual void update(double dt);

	// process input
	virtual void processInput(float dt);

	/*
	*	Rendering functions
	*/

	// set uniform shader varaibles (lighting, etc)
	virtual void renderShader(Shader shader, bool applyLighting = true);

	void renderInstances(std::string modelId, Shader shader);

	virtual void render();
	void renderText();

	/*
	* Game State methods
	*/

	void onGameOver();

	/*
	*	Model / instance methods
	*/

	// add entity to scene
	void addEntity(EntityBase* entity);

	// add model to scene models array
	void registerModel(Model* model);

	// generate an instance based on modelId
	RigidBody* generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot = glm::vec3(0.0f));

	void initInstances();

	void loadModels();

	void removeInstance( std::string instanceId);

	void markForDeletion( std::string instanceId);
	void clearDeadInstances();

	std::string currentId;
	std::string generateId();

	// collision methods
	void setBox(Box* box);
	Box* box;

	void checkCollision(double dt);

	std::vector<BoundingRegion> objects;
	std::queue<BoundingRegion> queue;

	void addToPending(RigidBody* instance);

	const int getPoints() const;

	void removeAxe();
	const unsigned int getAxes()const;
	void addAxes(const unsigned int axes);

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


private:
	void updateEntities(double dt);
	void updateBoundings(double dt);
	void updateInstancies(double dt);

	void handleCameraCollision(RigidBody& other);
	void handleAxeCollision(BoundingRegion& br);
	void handleProjectileCollision(BoundingRegion& br);

	unsigned int axes;

	unsigned int points;
	unsigned int lives;

	TextRenderer textRenderer;
	Shader textShader;

	BoundingRegion* cameraBR;
	BoundingRegion* ExitBR;
};
