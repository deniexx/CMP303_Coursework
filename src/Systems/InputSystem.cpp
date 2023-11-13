#include "InputSystem.h"

#include "../Core/Level.h"
#include <vector>
#include "../Core/Application.h"
#include <iostream>

void InputSystem::BeginSystem()
{
    // @TODO: Maybe initialize some values here
    // Not sure yet
}

void InputSystem::UpdateSystem(float deltaTime)
{
    std::shared_ptr<Level> level = Application::Instance->GetCurrentLevel();
    std::vector<Entity> players = level->GetAllPlayerEntities();

    bool bUpdate = false;
    for (auto& player : players)
    {
        if (!level->HasComponent<InputComponent>(player)) continue;
        NetworkPlayerComponent& networkPlayerComp = level->GetComponent<NetworkPlayerComponent>(player);
        if (networkPlayerComp.m_connectionType == PlayerConnectionType::ClientRemote) continue;
        if (networkPlayerComp.m_connectionType == PlayerConnectionType::None) continue; // this should never really be the case, but it's safer to check

        InputComponent& comp = level->GetComponent<InputComponent>(player);

        // Process input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
        {
            comp.m_moveInput = -1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
        {
            comp.m_moveInput = 1;
        }
		else
		{
            comp.m_moveInput = 0;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
        {
            comp.m_upDownNavigateInput = -1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
        {
            comp.m_upDownNavigateInput = 1;
        }
        else
        {
            comp.m_upDownNavigateInput = 0;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space))
        {
            comp.m_jumpInput = 1;
        }
        else
        {
            comp.m_jumpInput = 0;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::J))
        {
            comp.m_attackInput = 1;
        }
        else
        {
            comp.m_attackInput = 0;
        }
        // We do not care about these, as they are UI only (local player only)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Enter))
        {
            comp.m_confirmInput = 1;
        }
        else
        {
            comp.m_confirmInput = 0;
        }

        // @FIXME: This is wrong, we should check if the player is local or not and then process the input, because ideally all players will have an input component
        break;
    }
}

void InputSystem::DestroySystem()
{
    
}

void InputSystem::ClientUpdate()
{

}

void InputSystem::ServerUpdate()
{

}
