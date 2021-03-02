#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	static std::vector<Vertex> genList(float* vertices, int noVertices);
};

class Mesh
{
public:

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, aiColor4D diffuse, aiColor4D specular);

	void render(Shader shader);

	void cleanUp();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	aiColor4D diffuse;
	aiColor4D specular;
	// we could have more textures for model
	std::vector<Texture> textures;

private:
	unsigned int VBO, EBO;
	unsigned int VAO;

	bool noTex;

	void setup();
};

