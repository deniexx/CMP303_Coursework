#include "Level.h"

#include "TextureID.h"
#include "Application.h"
#include "../Messages/Messages.h"

void Level::Begin()
{
	gameTime = 0.f;
}

void Level::Update(float deltaTime)
{
	gameTime += deltaTime;
	// Update systems first
	for (auto& system : m_systems)
	{
		system->UpdateSystem(deltaTime);
	}

	// Send network updates after!
	// This was causing an issue when we sent messages right after updating systems
	for (auto& system : m_systems)
	{
		if (lastUpdatedSystemFrame[system.get()] + system->TimeBetweenUpdates() < GetElapsedTime())
		{
			lastUpdatedSystemFrame[system.get()] = GetElapsedTime();
			system->SendUpdate();
		}
	}

	for (int i = m_entities.size() - 1; i >= 0; --i)
	{
		if (!HasComponent<DeleteComponent>(m_entities[i])) continue;
		
		DeleteComponent& comp = GetComponent<DeleteComponent>(m_entities[i]);
		if (comp.m_markedForDelete)
		{
			DestroyEntity(m_entities[i]);
		}
	}
}

void Level::Render()
{
	for (auto entity : m_entities)
	{
		if (HasComponent<SpriteComponent>(entity))
		{		
			SpriteComponent& comp = GetComponent<SpriteComponent>(entity);
			TransformComponent& transform = GetComponent<TransformComponent>(entity);
			comp.m_sprite.setPosition(transform.m_x, transform.m_y);
			
			Application::Instance->m_window->draw(comp.m_sprite);
		}
		if (HasComponent<TextComponent>(entity))
		{
			TextComponent& comp = GetComponent<TextComponent>(entity);
			TransformComponent& transform = GetComponent<TransformComponent>(entity);
			comp.text.setPosition(transform.m_x, transform.m_y);

			Application::Instance->m_window->draw(comp.text);
		}
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

float Level::GetGameTime()
{
	return gameTime;
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
	EmplaceComponent<DeleteComponent>(internalPID);
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

	EmplaceComponent<SpriteComponent>(internalPID, sf::Color::Black);
	SpriteComponent& comp = GetComponent<SpriteComponent>(internalPID);
	
	comp.m_sprite.setTexture(Application::Instance->m_textureRegister[rahmiTexID], true);
	if (isServer)
		comp.m_sprite.setColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
	return internalPID;
}
void Level::DisconnectLocalPlayer()
{
	if (IsServer())
	{
		// Host left
		sf::Packet error;
		error << ERROR_EVENTID;
		error << "Host has left the game!";

		ServerSocketComponent& socketComp = GetComponent<ServerSocketComponent>(NETWORK_ENTITY);

		for (auto& socket : socketComp.m_tcpSockets)
		{
			socket->send(error);
		}

	}
	else
	{
		sf::Packet packet;
		packet << PLAYERLEFT_EVENTID;
		packet << GetLocalPlayerID();

		ClientSocketComponent& socketComp = GetComponent<ClientSocketComponent>(NETWORK_ENTITY);
		socketComp.m_tcpSocket.send(packet);
	}
}
#pragma endregion Noise
