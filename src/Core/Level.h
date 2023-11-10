#pragma once

#include "CoreDefines.h"

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include "../Components/Components.h"
#include "../Systems/ISystem.h"

class Level
{

	/* Interface */
public:
	virtual ~Level() = default;

	virtual void Begin();
	virtual void Update(float deltaTime);
	virtual void ClientUpdate();
	virtual void ServerUpdate();

	virtual void Render();

private:

	sf::Clock elapsedTimeClock;

#pragma region ECS
	/* ECS */
public:

	template<typename ComponentType>
	void AddComponent(Entity entityID, ComponentType component)
	{
		m_components[std::type_index(typeid(ComponentType))][entityID] = std::make_shared<ComponentType>(component);
	}

	template<typename ComponentType, typename... Args>
	ComponentType& EmplaceComponent(Entity entityID, Args&&... args)
	{
		std::shared_ptr<ComponentType> comp = std::make_shared<ComponentType>(std::forward<Args>(args)...);
		m_components[std::type_index(typeid(ComponentType))][entityID] = comp;
		return *comp;
	}

	template<typename ComponentType>
	ComponentType& GetComponent(Entity entityID)
	{
		auto& componentMap = m_components[std::type_index(typeid(ComponentType))];
		return *std::static_pointer_cast<ComponentType>(componentMap[entityID]);
	}

	template<typename ComponentType>
	bool HasComponent(Entity entityID)
	{
		auto& componentMap = m_components[std::type_index(typeid(ComponentType))];
		return componentMap.find(entityID) != componentMap.end();
	}

	template<typename ComponentType>
	Entity FindEntityFromComponent(const ComponentType& component)
	{
		auto& componentMap = m_components[std::type_index(typeid(ComponentType))];
		for (const auto& ceMapEntry : componentMap)
		{
			auto& storedComponent = *std::static_pointer_cast<ComponentType>(ceMapEntry.second);
			if (component = storedComponent)
			{
				return ceMapEntry.first;
			}
		}

		return -1;
	}

	template<typename ComponentType>
	std::vector<ComponentType> GetAllComponents()
	{
		std::vector<ComponentType> result;
		auto& componentMap = m_components[std::type_index(typeid(ComponentType))];
		for (auto& ceMapEntry : componentMap)
		{
			result.push_back(*std::static_pointer_cast<ComponentType>(ceMapEntry.second));
		}

		return result;
	}

	template<typename ComponentType>
	void RemoveComponent(Entity entityID)
	{
		auto& componentMap = m_components[std::type_index(typeid(ComponentType))];
		componentMap.erase(entityID);
	}

	std::vector<Entity> GetAllPlayerEntities()
	{
		std::vector<Entity> toReturn;
		for (uint8_t i = 1; i < playerCount + 1; ++i)
		{
			toReturn.push_back(i);
		}

		return toReturn;
	}

	std::vector<Entity> GetEntities()
	{
		return m_entities;
	}

	Entity CreatePlayer(int playerID = -1, std::string name = std::string());

	Entity CreateEntity(std::string name = std::string())
	{
		m_entities.push_back(++lastEntityID);
	    EmplaceComponent<TransformComponent>(lastEntityID, 0, 0);
		EmplaceComponent<TagComponent>(lastEntityID, name);
		EmplaceComponent<UUIDComponent>(lastEntityID, elapsedTimeClock.getElapsedTime().asMilliseconds());
		return lastEntityID;
	}

	void DestroyEntity(Entity entityID)
	{
		for (auto& componentMap : m_components) {
			componentMap.second.erase(entityID);
		}
		m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entityID), m_entities.end());
	}

private:

	// @TODO: IDs 1-6(6), will be for players so we should start at 6
	// @TODO: We will only create players, if the start game has been called, or the server has sent us a create a player event
	// @TODO: Server/Client server systems that do work on the components
	// @TODO: Figure out what events should we call and their IDs, make sure they are well defined and get ReplicatedStructs to hold that data!
	// @TODO: Variable length messages for strings?
	// @TODO: Maybe figure out more of the game architecture before proceeding
	// @TODO: Figure out animation
	// ---------------------------- FOR THIS WEEK ----------------------------
	// @TODO: Start with rendering the world and platforms
	// @TODO: Implement the physics and input systems and get players moving on the screen with velocity
	// @TODO: Add hit system and hit components that would check if you have been hit or not and the hit system will deal with the hits accordingly
	// @TODO: ApplyImpulse on enemies after being hit
	// @TODO: Good enough for the week
	// @TODO: Possibly implement a second player on the screen
    // This will be pre-incremented, starting from 1 as 0 will be our networking entity, handling the network messages
    uint8_t lastPlayerID = 0; // Only used on the server
	uint8_t playerCount = 0; // The number of players, used for some systems
	uint8_t localPlayerID = -1;
	uint32_t lastEntityID = 6;
	std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<void>>> m_components;
	std::vector<Entity> m_entities;
	std::vector<std::unique_ptr<ISystem>> m_systems;

#pragma endregion ECS
};

