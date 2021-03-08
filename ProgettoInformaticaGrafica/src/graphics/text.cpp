#include "text.h"

#include <iostream>

TextRenderer::TextRenderer(const char* path, FT_UInt size)
	:fontPath(path), size(size)
{ }

bool TextRenderer::init()
{
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "Could not init FreeType Library" << std::endl;
		return false;
	}

	FT_Face face;
	if (FT_New_Face(ft, fontPath, 0, &face))
	{
		std::cout << "Failed to load font" << std::endl;
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, size); // set each pixel font size

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Characters[c] = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(unsigned int)face->glyph->advance.x
		};
	}

	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// configure VAO/VBO for texture quads
	// -----------------------------------
	VAO.generate();
	VAO.bind();

	VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["VBO"].generate();
	VAO["VBO"].bind();
	// 2D quad requires 6 vertices of 4 float (2 for pos, 2 for tex coords)
	VAO["VBO"].setData<GLfloat>(6 * 4, NULL, GL_DYNAMIC_DRAW);
	VAO["VBO"].setAttPointer<GLfloat>(0, 4, GL_FLOAT, 4, 0);
	VAO["VBO"].clear();
	ArrayObject::clear();

	return true;
}

void TextRenderer::render(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	// activate corresponding render state	
	shader.activate();
	shader.set3Float("textColor", color);
	shader.setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	VAO.bind();

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		// update VBO for each character
		float vertices[6 * 4] = {
			 xpos,     ypos + h,   0.0f, 0.0f ,
			 xpos,     ypos,       0.0f, 1.0f ,
			 xpos + w, ypos,       1.0f, 1.0f ,

			 xpos,     ypos + h,   0.0f, 0.0f ,
			 xpos + w, ypos,       1.0f, 1.0f ,
			 xpos + w, ypos + h,   1.0f, 0.0f
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// update content of VBO memory
		VAO["VBO"].bind();
		VAO["VBO"].updateData<GLfloat>(0, 6 * 4, vertices);
		// render quad
		VAO.draw(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}

	ArrayObject::clear();
	glBindTexture(GL_TEXTURE_2D, 0);
}

