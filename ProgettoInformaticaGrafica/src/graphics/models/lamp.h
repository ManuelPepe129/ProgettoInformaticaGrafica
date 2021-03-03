#pragma once



#include "cube.h"
#include "../light.h"
#include "../modelArray.h"
#include "../material.h"
#include "../shader.h"

#include <glm/glm.hpp>

class Lamp : public Cube
{
public:

    Lamp(glm::vec3 lightColor = glm::vec3(1.0f),
		glm::vec4 ambient = glm::vec4(1.0f),
		glm::vec4 diffuse = glm::vec4(1.0f),
		glm::vec4 specular = glm::vec4(1.0f),
		float c0 = 1.0f,
		float c1 = 0.07f,
		float c2 = 0.017f,
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 size = glm::vec3(1.0f));

    //Lamp();

    void render(Shader shader, float dt, Box* box, bool setModel = true, bool doRender = true);

    glm::vec3 lightColor;
    
    PointLight pointLight;
};

class LampArray : public ModelArray<Lamp> {
public:
	std::vector<PointLight> lightInstances;

    void init();

    void render(Shader shader, float dt, Box* box);
};