#include "Level.h"
#include "Application.h"
#include <iostream>
#include "../Systems/InputSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/HitSystem.h"

void Level::Begin()
{
	m_systems.push_back(std::make_unique<InputSystem>());
	m_systems.push_back(std::make_unique<PhysicsSystem>());
	m_systems.push_back(std::make_unique<HitSystem>());

	if (server)
	{
		tcplistener.listen(28000, "127.0.0.1");

		localPlayerID = CreatePlayer(-1, std::to_string(lastPlayerID + 1));
		UUIDComponent& uuidComp = GetComponent<UUIDComponent>(localPlayerID);
		TagComponent& tagComp = GetComponent<TagComponent>(localPlayerID);
		TransformComponent& transComp = GetComponent<TransformComponent>(localPlayerID);
		transComp.m_x = 100.f * 1.77777777778f;
		transComp.m_y = 100.f;

		std::cout << tagComp.m_tag << ": " << uuidComp.m_uuid << "\n";
	}
	else
	{
		tcpsocket1.connect("127.0.0.1", 28000);
	}

	tcplistener.setBlocking(false);
}

void Level::Update(float deltaTime)
{
	for (auto& system : m_systems)
	{
		system->UpdateSystem(deltaTime);
	}

	if (server && tcplistener.accept(tcpsocket1) == sf::Socket::Done)
	{
		CreatePlayer();
		sf::Packet packet;
		packet << "Hello!";
		tcpsocket1.send(packet);
		// @TODO: Send a hello message back and forth, to confirm status
		// Only create a character, if we have received a confirmation back
		// @TODO: Check for inactivity and connection stability
	}
	else if (!server)
	{
		sf::Packet packet;
		if (tcpsocket1.receive(packet) == sf::Socket::Status::Done)
		{
			std::string helloStr;
			packet >> helloStr;
			std::cout << helloStr;
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

double Level::GetElapsedTime()
{
	return elapsedTimeClock.getElapsedTime().asSeconds();
}

Entity Level::CreatePlayer(int playerID, std::string name, bool localPlayer)
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
	EmplaceComponent<HitComponent>(internalPID);
	PlayerConnectionType type;

	if (isServer)
	{
		type = PlayerConnectionType::Server;
	}
	else if (localPlayer)
	{
		type = PlayerConnectionType::ClientLocal;
		localPlayerID = internalPID;
	}
	else
	{
		type = PlayerConnectionType::ClientRemote;
		// @TODO: Find out how to determine if the client is local or not, if it is a local client create an input component, else do not
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
