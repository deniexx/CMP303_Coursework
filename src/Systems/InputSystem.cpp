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
    Level* level = Application::Instance->GetCurrentLevel().get();
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

void InputSystem::SendUpdate()
{
	Level* level = Application::Instance->GetCurrentLevel().get();
    if (!level->IsServer())
    {
        if (!level->HasComponent<InputComponent>(level->GetLocalPlayerID())) return;

        InputComponent& inputComponent = level->GetComponent<InputComponent>(level->GetLocalPlayerID());
        InputUpdateMessage message;
        message.m_playerID = level->GetLocalPlayerID();
        message.m_moveInput = inputComponent.m_moveInput;
        message.m_attackInput = inputComponent.m_attackInput;
        message.m_jumpInput = inputComponent.m_jumpInput;

        sf::Packet packet;
        packet << INPUTUPDATE_EVENTID;
        packet << message;

        ClientSocketComponent& socketComponent = level->GetComponent<ClientSocketComponent>(NETWORK_ENTITY);
        socketComponent.m_tcpSocket.send(packet);
    }
    else
    {
        std::vector<Entity> players = level->GetAllPlayerEntities();
        
        sf::Packet packet;
        for (Entity player : players)
        {
			InputComponent& inputComponent = level->GetComponent<InputComponent>(level->GetLocalPlayerID());
			InputUpdateMessage message;
            message.m_playerID = player;
			message.m_moveInput = inputComponent.m_moveInput;
			message.m_attackInput = inputComponent.m_attackInput;
			message.m_jumpInput = inputComponent.m_jumpInput;

			sf::Packet packet;
            packet << INPUTUPDATE_EVENTID;
			packet << message;
        }

		const ServerSocketComponent& serverSocketComponent = level->GetComponent<ServerSocketComponent>(NETWORK_ENTITY);
		for (sf::TcpSocket* socket : serverSocketComponent.m_tcpSockets)
		{
			socket->send(packet);
		}
    }
}
