#pragma once

#include <glm/glm.hpp>

#include "shader.h"

struct PointLight {
	glm::vec3 position;

	// attenuation constants
	float c0;
	float c1;
	float c2;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};

struct DirectionalLight {
	glm::vec3 direction;

	// attenuation constants
	float c0;
	float c1;
	float c2;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader);
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;

	float cutOff;
	float outerCutOff;

	// attenuation constants
	float c0;
	float c1;
	float c2;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader shader, int idx);
};