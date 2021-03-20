#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include "basescene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

enum class MenuState
{
	MAIN_MENU,
	NEW_GAME,
	CREDITS,
	RULES
};

class Menu : public BaseScene
{
public:
	Menu(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight);

	void init();

	virtual void render();

	// update screen before each frame
	void update();

	virtual void cleanup();

	bool buttonCentered(const char* label, float alignment = 0.5f);

	MenuState GetState() {
		return currentMenuState;
	}

private:
	MenuState currentMenuState;
};

