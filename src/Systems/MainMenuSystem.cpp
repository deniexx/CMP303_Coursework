#include "MainMenuSystem.h"

#include <SFML/Window/Keyboard.hpp>

#include "../Core/Application.h"

void MainMenuSystem::BeginSystem()
{
}

void MainMenuSystem::UpdateSystem(float deltaTime)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::H))
    {
        Application::Instance->m_isServer = true;
        Application::Instance->SwitchLevel(GameLevel::Gameplay);
        return;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::J))
    {
        Application::Instance->m_isServer = false;
        Application::Instance->SwitchLevel(GameLevel::Gameplay);
    }
}

void MainMenuSystem::DestroySystem()
{
}

float MainMenuSystem::TimeBetweenUpdates()
{
    return 9999999999999.f;
}

void MainMenuSystem::SendUpdate()
{
}
