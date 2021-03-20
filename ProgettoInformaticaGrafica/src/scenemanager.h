#pragma once

#include <stack>

#include "graphics/basescene.h"

class SceneManager
{
public:
	static SceneManager* GetManager();
protected:
	SceneManager();
private:
	std::stack<BaseScene> ScenesStack;
	static SceneManager* Manager;
};

