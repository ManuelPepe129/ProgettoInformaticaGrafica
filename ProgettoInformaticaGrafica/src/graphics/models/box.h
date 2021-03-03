#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "../../algorithms/bounds.h"
#include "../shader.h"

#define UPPER_BOUND 100

class Box {
public:
    std::vector<glm::vec3> offsets;
    std::vector<glm::vec3> sizes;

    void init();

    void render(Shader shader);

    void addInstance(BoundingRegion br, glm::vec3 pos, glm::vec3 size);

    void cleanup();

private:
    unsigned int VAO, VBO, EBO;
    unsigned int offsetVBO, sizeVBO;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};
