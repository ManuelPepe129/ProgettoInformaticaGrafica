#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
	Mesh();

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	void render(Shader shader);

	void cleanUp();

	void setup();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	
	// we could have more textures for model
	std::vector<Texture> textures;
private:
	unsigned int VBO, EBO;
	unsigned int VAO;

};

