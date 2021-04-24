#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "mesh.h"

#include "models/box.hpp"

#include "../physics/rigidbody.h"
#include "../algorithms/bounds.h"
#include "material.h"

// model switches
#define DYNAMIC				(unsigned int)1
#define CONST_INSTANCES		(unsigned int)2 //  10
#define NO_TEX				(unsigned int)4	// 100

class Scene; // forward declaration

class Model {
public:
	std::string id;

	RigidBody rb;

	BoundTypes boundType;

	std::vector<Mesh> meshes;
	std::vector<BoundingRegion> boundingRegions;

	std::vector<RigidBody*> instances;

	//unsigned int maxNoInstances;
	unsigned int currentNoInstances;

	unsigned int switches;

	Model(std::string id, BoundTypes boundType, unsigned int flags = 0);

	// initialize method
	virtual void init();

	RigidBody* generateInstance(glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot);

	void initInstances();

	void loadModel(const std::string& path);

	void setMaterial(Material material);

	virtual void update(double dt);

	virtual void render(Shader shader, bool setModel = true);

	void cleanup();

	glm::vec3 getPosition();

	glm::vec3 getSize();

	void removeInstance(unsigned int idx);

	void removeInstance(std::string instanceId);

	unsigned int getIdx(std::string id);

protected:
	//bool noTex;

	std::string directory;

	std::vector<Texture> textures_loaded;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);

	// VBOs for model matrices
	BufferObject modelVBO;
	BufferObject normalModelVBO;
};
