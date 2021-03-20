#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>

#include "graphics/light.h"
#include "graphics/shader.h"
#include "graphics/basescene.h"
#include "graphics/models/lamp.hpp"
#include "graphics/models/box.hpp"
#include "graphics/model.h"

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

#include "algorithms/states.hpp"

#include "scene.h"

class GameScene :
    public Scene
{
public:
    GameScene(int glfwVersionMajor, int glfwVersionMinor,
        const char* title, unsigned int scrWidth, unsigned int scrHeight);

    virtual bool init();

    virtual void render();

    virtual void cleanup();

private:
    std::vector<Model> models;

    // SHADERS===============================
    Shader shader;
    Shader lampShader;
    Shader boxShader;

    Box box;

    LampArray lamps;
};

