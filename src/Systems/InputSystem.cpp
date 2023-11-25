#include "InputSystem.h"

#include "../Core/Level.h"
#include <vector>
#include "../Core/Application.h"
#include <iostream>
#include "../Messages/Messages.h"

void InputSystem::BeginSystem()
{
    // @TODO: Maybe initialize some values here
    // Not sure yet
}

void InputSystem::UpdateSystem(float deltaTime)
{
    if (!Application::Instance->IsFocused()) return;
    
    Level* level = Application::Instance->GetCurrentLevel().get();
    std::vector<Entity> players = level->GetAllPlayerEntities();
    
    for (auto& player : players)
    {
        if (!level->HasComponent<InputComponent>(player)) continue;
        if (!level->IsLocalPlayer()) continue;

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

        InputArray& inputArray = level->GetComponent<InputArray>(player);
        inputArray.m_inputs.push_back(comp);
        break;
    }
}

void InputSystem::DestroySystem()
{
    
}

void InputSystem::SendUpdate()
{
	Level* level = Application::Instance->GetCurrentLevel().get();
    
    if (!level->HasComponent<InputArray>(level->GetLocalPlayerID())) return;

    InputArray& inputArr = level->GetComponent<InputArray>(level->GetLocalPlayerID());
    
    sf::Packet packet;
    packet << INPUTUPDATE_EVENTID;
    packet << level->GetLocalPlayerID();
    packet << inputArr;

    if (!level->IsServer())
    {
        ClientSocketComponent& socketComponent = level->GetComponent<ClientSocketComponent>(NETWORK_ENTITY);
        socketComponent.m_tcpSocket.send(packet);
    }
    else
    {
        ServerSocketComponent& socketComponent = level->GetComponent<ServerSocketComponent>(NETWORK_ENTITY);

        for (sf::TcpSocket* socket : socketComponent.m_tcpSockets)
        {
            socket->send(packet);
        }
    }
    inputArr.m_inputs.clear();

}
