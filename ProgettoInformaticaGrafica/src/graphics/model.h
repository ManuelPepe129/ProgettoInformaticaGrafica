#ifndef OBJECT_H
#define OBJECT_H

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

enum class ModelType {
	WALL,
	COLLECTABLE
};

class Model {
public:
	RigidBody rb;
	glm::vec3 size;

	ModelType type;
	BoundTypes boundType;

	std::vector<Mesh> meshes;

	bool toBeDestroyed;

	Model(BoundTypes boundType = BoundTypes::AABB, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f), bool noTex = false, ModelType type = ModelType::WALL);

	void loadModel(std::string path);

	void render(Shader shader, float dt, Box *box, bool setModel = true, bool doRender = true);

	void render(Shader shader, Box* box, bool setModel = true, bool doRender = true);

	void cleanup();

protected:

	bool noTex;
	
	std::string directory;

	std::vector<Texture> textures_loaded;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);
};

#endif