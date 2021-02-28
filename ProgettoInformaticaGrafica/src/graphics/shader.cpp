#include "Shader.h"

Shader::Shader()
{
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	generate(vertexShaderPath, fragmentShaderPath);
}

void Shader::generate(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	int success;
	char infoLog[512];

	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	// catch errors
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Error linking shaders:" << std::endl << infoLog << std::endl;
	}

	// we can delete shaders once linked into the program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::activate()
{
	glUseProgram(id);
}

const unsigned int Shader::GetId() const
{
	return id;
}

std::string Shader::loadShaderSrc(const char* filepath)
{
	std::ifstream file;
	std::stringstream buffer;

	std::string ret = "";

	file.open(filepath);

	if (file.is_open())
	{
		buffer << file.rdbuf();
		ret = buffer.str();
	}
	else
	{
		std::cout << "Could not open shader file " << filepath;
	}

	return ret;
}

GLuint Shader::compileShader(const char* filepath, GLenum type)
{
	int success;
	char infoLog[512];

	/* Shaders */
	GLuint shader;
	shader = glCreateShader(type);
	std::string shaderSrc = loadShaderSrc(filepath);
	const GLchar* shaderCode = shaderSrc.c_str();
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	// catch errors if any
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error with shader comp.:" << std::endl << infoLog << std::endl;
	}
	return shader;
}

void Shader::setBool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set3Float(const std::string& name, glm::vec3 v) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

/*
void Shader::set4Float(const std::string& name, aiColor4D color) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), color.r, color.g, color.b, color.a);
}
*/

void Shader::set4Float(const std::string& name, glm::vec4 v) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setMat3(const std::string& name, glm::mat3 val) {
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}


void Shader::setMat4(const std::string& uniformName, glm::mat4 transform)
{
	glUniformMatrix4fv(glGetUniformLocation(id, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(transform));
}
