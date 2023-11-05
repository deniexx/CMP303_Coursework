#include "Level.h"
#include "Application.h"
#include <iostream>

void Level::Begin()
{
	Entity ent = CreateEntity("Player");

	AddComponent<SpriteComponent>(ent, sf::Color::Red);

	SpriteComponent& comp = GetComponent<SpriteComponent>(ent);
	sf::Texture texture;
	texture.create(32, 32);
	comp.m_sprite.setTexture(texture, true);
}

void Level::Update(float deltaTime)
{
	for (auto entity : m_entities)
	{
        std::cout << deltaTime << "\n";
		TransformComponent& comp = GetComponent<TransformComponent>(entity);
		comp.m_x += deltaTime * 5.f;
		comp.m_y += deltaTime * 5.f;
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
