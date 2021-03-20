#include "scenemanager.h"

SceneManager* SceneManager::Manager = nullptr;

SceneManager* SceneManager::GetManager()
{
    if (SceneManager::Manager == nullptr)
    {
        SceneManager::Manager = new SceneManager;
    }
    return SceneManager::Manager;
}

SceneManager::SceneManager()
{ }
