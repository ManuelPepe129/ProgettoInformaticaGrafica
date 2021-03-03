#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "../algorithms/bounds.h"

class Model
{
public:
	Model() {};
	Model(BoundTypes boundType = BoundTypes::AABB, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f), bool noTex = false);

	bool loadModel(std::string path);

	void render(Shader shader);

	void cleanup();

	glm::vec3 pos;
	glm::vec3 size;

	BoundTypes boundType;

protected:
	bool noTex;
	std::vector<Mesh> meshes;
	std::string directory;

	std::vector<Texture> texturesLoaded;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);
};

