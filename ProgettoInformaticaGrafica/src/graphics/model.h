#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "models/box.h"
#include "../physics/rigidbody.h"
#include "../algorithms/bounds.h"

class Model
{
public:
	Model(BoundTypes boundType = BoundTypes::AABB, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f), bool noTex = false);

	bool loadModel(std::string path);

	void render(Shader shader, float dt, Box* box, bool setModel = true, bool doRender = true);

	void cleanup();

	glm::vec3 size;

	std::vector<Mesh> meshes;

	RigidBody rb;

	BoundTypes boundType;

protected:
	bool noTex;
	
	std::string directory;

	std::vector<Texture> texturesLoaded;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);
};

