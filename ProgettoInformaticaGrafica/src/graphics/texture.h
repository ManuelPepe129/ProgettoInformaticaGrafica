#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>

#include <stb/stb_image.h>

class Texture
{
public:
	Texture();
	Texture(std::string dir, std::string path, aiTextureType type);

	void generate();
	bool load(bool flip = true);
	void active();
	void bind();

	unsigned int id;
	aiTextureType type;
	std::string dir;
	std::string path;

};

