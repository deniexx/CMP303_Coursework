#include "Level.h"

#include <iostream>

#include "Application.h"
#include "../Systems/InputSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/HitSystem.h"
#include "../Systems/NetworkSystem.h"

void Level::Begin()
{
	m_entities.push_back(NETWORK_ENTITY);

#if DEBUG
	std::cout << "Server\n";
	CreatePlayer();
	EmplaceComponent<ServerSocketComponent>(NETWORK_ENTITY);
	EmplaceComponent<LastPhysicsState>(NETWORK_ENTITY);
#else
	std::cout << "Client\n";
	EmplaceComponent<ClientSocketComponent>(NETWORK_ENTITY);
#endif

	AddSystem<NetworkSystem>();
	AddSystem<InputSystem>();
	AddSystem<PhysicsSystem>();
	AddSystem<HitSystem>(); // @TODO: When abstracting this, make sure to only keep the relevant systems we really require
}

void Level::Update(float deltaTime)
{
	for (auto& system : m_systems)
	{
		system->UpdateSystem(deltaTime);

		if(lastUpdatedSystemFrame[system.get()] + system->TimeBetweenUpdates() < GetElapsedTime())
		{
			lastUpdatedSystemFrame[system.get()] = GetElapsedTime();
			system->SendUpdate();
		}
	}
}

void Level::ClientUpdate()
{

}

void Level::ServerUpdate()
{

}

void Level::Render()
{
	for (auto entity : m_entities)
	{
		if (!HasComponent<SpriteComponent>(entity)) continue;
		
		SpriteComponent& comp = GetComponent<SpriteComponent>(entity);
		TransformComponent& transform = GetComponent<TransformComponent>(entity);
		comp.m_sprite.setPosition(transform.m_x, transform.m_y);
		
		Application::Instance->m_window->draw(comp.m_sprite);
	}
}

#pragma region Noise
float Level::GetElapsedTime()
{
	return elapsedTimeClock.getElapsedTime().asSeconds();
}

bool Level::IsServer()
{
	return HasComponent<ServerSocketComponent>(NETWORK_ENTITY);
}

bool Level::IsLocalPlayer()
{
	return IsEntityLocalPlayer(localPlayerID);
}

bool Level::IsEntityLocalPlayer(Entity entity)
{
	if (!HasComponent<NetworkPlayerComponent>(entity))
		return false;

	NetworkPlayerComponent& comp = GetComponent<NetworkPlayerComponent>(entity);
	
	bool local = false;
	local |= comp.m_connectionType == PlayerConnectionType::Server;
	local |= comp.m_connectionType == PlayerConnectionType::ClientLocal;

	return local;
}

Entity Level::CreatePlayer(int playerID, std::string name, bool localPlayer)
{
	if (lastPlayerID > 6 || playerCount > 6)
		return -1; // Do not create more than 6 players, so we return an invalid ID, this shouldn't really ever happen
				   // because it is being called by the server/client through a message

	const bool isServer = playerID == -1; // if playerID is -1, it means we are using lastPlayerID, hence we are the server
	if (isServer)
	{
		++lastPlayerID;
	}
	const uint8_t internalPID = isServer ? lastPlayerID : playerID;

	++playerCount;
	m_entities.push_back(internalPID);
	EmplaceComponent<TransformComponent>(internalPID, 0, 0);
	EmplaceComponent<TagComponent>(internalPID, name);
	EmplaceComponent<UUIDComponent>(internalPID, elapsedTimeClock.getElapsedTime().asMilliseconds());
	EmplaceComponent<MovementComponent>(internalPID);
	EmplaceComponent<HitComponent>(internalPID);
	EmplaceComponent<InputArray>(internalPID);
	PlayerConnectionType type;

	if (isServer && internalPID == 1) // this means we are the server, since he'll always be entity 1
	{
		type = PlayerConnectionType::Server;
		localPlayerID = internalPID;
	}
	else if (localPlayer)
	{
		type = PlayerConnectionType::ClientLocal;
		localPlayerID = internalPID;
	}
	else
	{
		type = PlayerConnectionType::ClientRemote;
	}

	EmplaceComponent<InputComponent>(internalPID);
	EmplaceComponent<NetworkPlayerComponent>(internalPID, type);

	/* TEMPORARY*/
	EmplaceComponent<SpriteComponent>(internalPID, sf::Color::Black);
	SpriteComponent& comp = GetComponent<SpriteComponent>(internalPID);
	sf::Texture texture;
	texture.create(32, 32);
	comp.m_sprite.setTexture(texture, true);
	return internalPID;
}
#pragma endregion Noise
