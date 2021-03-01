#pragma once

#include "cube.h"
#include "../light.h"

class Lamp : public Cube
{
public:

    Lamp(glm::vec3 pos,
        glm::vec3 size,
        glm::vec3 lightColor,
        float c0,
        float c1,
        float c2,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular);

    Lamp();

    void render(Shader shader);

    glm::vec3 lightColor;
    
    PointLight pointLight;
};

