#include "Level.h"
#include "Application.h"
#include <iostream>

void Level::Begin()
{
	for (int i = 0; i < 10; ++i)
	{
		std::string playerName = "Player" + std::to_string(i);
		Entity ent = CreateEntity(playerName);
		EmplaceComponent<SpriteComponent>(ent, sf::Color::Black);
		SpriteComponent& comp = GetComponent<SpriteComponent>(ent);
		sf::Texture texture;
		texture.create(32, 32);
		comp.m_sprite.setTexture(texture, true);

		TransformComponent& tranComp = GetComponent<TransformComponent>(ent);
		UUIDComponent& uuidComp = GetComponent<UUIDComponent>(ent);
		TagComponent& tagComp = GetComponent<TagComponent>(ent);

		tranComp.m_x = 0 + (i * 160.f);
		tranComp.m_y = 0 + (i * 90.f);
		std::cout << tagComp.m_tag << ": " << uuidComp.m_uuid << "\n";
	}
}

void Level::Update(float deltaTime)
{
	for (auto entity : m_entities)
	{
		TransformComponent& comp = GetComponent<TransformComponent>(entity);
		comp.m_x += deltaTime * 10.f * 1.77777777778f;
		comp.m_y += deltaTime * 10.f;
	}
}

void Level::Render()
{
	for (auto entity : m_entities)
	{
		if (!HasComponent<SpriteComponent>(entity)) continue;
		
		SpriteComponent& comp = GetComponent<SpriteComponent>(entity);
		TransformComponent& transform = GetComponent<TransformComponent>(entity);
		comp.m_sprite.setPosition(transform.m_x, transform.m_y);
	
		Application::Instance->window->draw(comp.m_sprite);
	}
}
