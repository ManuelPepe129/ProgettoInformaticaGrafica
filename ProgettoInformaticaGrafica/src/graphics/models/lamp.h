#pragma once

#include "cube.h"

class Lamp :
    public Cube
{
public:

    Lamp(glm::vec3 pos, glm::vec3 size, glm::vec3 lightColor, glm::vec3 ambient,glm::vec3 diffuse, glm::vec3 specular);

    void render(Shader shader);

    glm::vec3 lightColor;
    
    // light strength values
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

