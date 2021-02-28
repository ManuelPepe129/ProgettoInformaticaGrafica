#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

class Shader
{
public:

	Shader();

	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

	void generate(const char* vertexShaderPath, const char* fragmentShaderPath);

	void activate();
	const unsigned int GetId() const;

	// uniform functions
	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void set3Float(const std::string& name, float v1, float v2, float v3);
	void set3Float(const std::string& name, glm::vec3 v);
	void set4Float(const std::string& name, float v1, float v2, float v3, float v4);
	//void set4Float(const std::string& name, aiColor4D color);
	void set4Float(const std::string& name, glm::vec4 v);
	void setMat3(const std::string& name, glm::mat3 val);
	void setMat4(const std::string& uniformName, glm::mat4 transform);

private:

	unsigned int id;

	// utility functions
	std::string loadShaderSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);
};

