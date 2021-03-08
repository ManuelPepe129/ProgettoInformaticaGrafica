#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include <ft2build.h>
#include <map>
#include "glmemory.hpp"
#include FT_FREETYPE_H

struct Character {
	unsigned int textureID;  // ID handle of the glyph texture
	glm::ivec2   size;       // size of glyph
	glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
	unsigned int advance;    // Offset to advance to next glyph
};

class TextRenderer
{
public:
	TextRenderer(const char* path, FT_UInt size);

	bool init();

	void render(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

private:

	const char* fontPath;

	ArrayObject VAO;

	FT_Library ft;
	FT_Face face;

	FT_UInt size;

	std::map<GLchar, Character> Characters;
};

