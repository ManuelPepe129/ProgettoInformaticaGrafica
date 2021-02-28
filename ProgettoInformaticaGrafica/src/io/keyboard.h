#pragma once

#include <glad/glad.h>
#include<GLFW/glfw3.h>

class Keyboard
{
public:
	// key state callback
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	/*
		accessors
	*/

	static bool key(int key);
	static bool keyChanged(int key);
	static bool keyReleased(int key);
	static bool keyPressed(int key);

private:
	static bool keys[];
	static bool keysChanged[];
};

