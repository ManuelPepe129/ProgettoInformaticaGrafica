#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"

class Model
{
public:
	Model();

	void init();

	void render(Shader shader);

	void cleanup();

	std::vector<Mesh> meshes;
};

