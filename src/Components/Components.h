#pragma once

#include "SFML/Graphics.hpp"

struct TransformComponent
{
	TransformComponent() = default;
	TransformComponent(TransformComponent& other) = default;
	TransformComponent(float x, float y) : m_x(x), m_y(y) {}

	float m_x = 0, m_y = 0;
};

struct SpriteComponent
{
	SpriteComponent() = default;
	SpriteComponent(SpriteComponent& other) = default;
	SpriteComponent(sf::Sprite& sprite) : m_sprite(sprite) {}
	SpriteComponent(sf::Sprite sprite) : m_sprite(sprite) {}
	SpriteComponent(sf::Color colour)
	{
		m_sprite = sf::Sprite(sf::Texture());
		m_sprite.setColor(colour);
	}

	sf::Sprite m_sprite;
};

struct TagComponent
{
	TagComponent() = default;
	TagComponent(TagComponent & other) = default;
	TagComponent(std::string tag) : m_tag(tag) {}

	std::string m_tag;
};
