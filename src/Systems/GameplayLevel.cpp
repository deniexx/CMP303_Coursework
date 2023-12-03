#include "GameplayLevel.h"

#include <iostream>

#include "../Core/Application.h"
#include "../Systems/InputSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/HitSystem.h"
#include "../Systems/NetworkSystem.h"
#include "../Systems/DeathSystem.h"

void GameplayLevel::Begin()
{
    Level::Begin();

    m_entities.push_back(NETWORK_ENTITY);

    if (Application::Instance->m_isServer)
    {
        std::cout << "Server\n";
        CreatePlayer();
        EmplaceComponent<ServerSocketComponent>(NETWORK_ENTITY);
        EmplaceComponent<LastPhysicsState>(NETWORK_ENTITY);
    }
    else
    {
        std::cout << "Client\n";
        EmplaceComponent<ClientSocketComponent>(NETWORK_ENTITY);
    }

    AddSystem<NetworkSystem>();
    AddSystem<InputSystem>();
    AddSystem<PhysicsSystem>();
    AddSystem<HitSystem>();
    AddSystem<DeathSystem>();
}
