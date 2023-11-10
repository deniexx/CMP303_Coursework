#include "Level.h"
#include "Application.h"
#include <iostream>
#include "../Systems/InputSystem.h"
#include "../Systems/PhysicsSystem.h"

void Level::Begin()
{
	m_systems.push_back(std::make_unique<InputSystem>());
	m_systems.push_back(std::make_unique<PhysicsSystem>());

	for (int i = 1; i < 3; ++i)
	{
		Entity ent = CreatePlayer(i, "Player" + std::to_string(i));
		UUIDComponent& uuidComp = GetComponent<UUIDComponent>(ent);
		TagComponent& tagComp = GetComponent<TagComponent>(ent);
		TransformComponent& transComp = GetComponent<TransformComponent>(ent);
		transComp.m_x = (i) * 100.f * 1.77777777778f;
		transComp.m_y = (i) * 100.f;

		std::cout << tagComp.m_tag << ": " << uuidComp.m_uuid << "\n";
	}
}

void Level::Update(float deltaTime)
{
	for (auto& system : m_systems)
	{
		system->UpdateSystem(deltaTime);
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

Entity Level::CreatePlayer(int playerID, std::string name)
{
	if (lastPlayerID > 6 || playerCount > 6)
		return -1; // Do not create more than 6 players, so we return an invalid ID, this shouldn't really ever happen
				   // because it is being called by the server/client through a message

	bool isServer = playerID == -1; // if playerID is -1, it means we are using lastPlayerID, hence we are the server
	if (isServer)
	{
		++lastPlayerID;
	}
	int internalPID = isServer ? lastPlayerID : playerID;

	++playerCount;
	m_entities.push_back(internalPID);
	EmplaceComponent<TransformComponent>(internalPID, 0, 0);
	EmplaceComponent<TagComponent>(internalPID, name);
	EmplaceComponent<UUIDComponent>(internalPID, elapsedTimeClock.getElapsedTime().asMilliseconds());
	EmplaceComponent<MovementComponent>(internalPID);
	PlayerConnectionType type;

	if (isServer)
	{
		type = PlayerConnectionType::Server;
	}
	else /*if (isLocalPlayer)*/
	{
		type = PlayerConnectionType::ClientLocal;
	}/*else
	{
		type = PlayerConnectionType::ClientRemote;
		// @TODO: Find out how to determine if the client is local or not, if it is a local client create an input component, else do not
	}*/

	if (type == PlayerConnectionType::Server || type == PlayerConnectionType::ClientLocal)
	{
		EmplaceComponent<InputComponent>(internalPID);
	}

	EmplaceComponent<NetworkPlayerComponent>(internalPID, type);

	/* TEMPORARY*/
	EmplaceComponent<SpriteComponent>(internalPID, sf::Color::Black);
	SpriteComponent& comp = GetComponent<SpriteComponent>(internalPID);
	sf::Texture texture;
	texture.create(32, 32);
	comp.m_sprite.setTexture(texture, true);
	return internalPID;
}
