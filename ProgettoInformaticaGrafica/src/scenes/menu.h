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
	GET_NAME,
	NEW_GAME,
	LEADERBOARD,
	CREDITS,
	RULES
};

struct Score
{
	std::string name;
	std::string time;
	std::string points;
};

class Menu : public BaseScene
{
public:
	Menu(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight);

	virtual bool init();

	virtual void render();

	virtual void processInput(float dt);

	// update screen before each frame
	void update();

	virtual void cleanup();

	MenuState currentMenuState;

private:
	bool buttonCentered(const char* label, float alignment = 0.5f);

	std::vector<Score> scores;

	void sortScores();

	void textCentered(std::string text);

	bool textInputCentered(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags=0);

	
};

