#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>

class Texture
{
public:
	Texture();
	Texture(const char* path, const char* name, bool defaultParams = true);

	void generate();
	bool load(bool flip = true);
	void active();
	void bind();

	void setFilters(GLenum all);
	void setFilters(GLenum mag, GLenum min);

	void setWrap(GLenum all);
	void setWrap(GLenum s, GLenum t);

	// texture object
	int id;
	unsigned int tex;
	const char* name;

private:
	static int currentId;

	const char* path;
	int height;
	int width;
	int nChannels;
};

