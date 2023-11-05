#pragma once

#include "SFML/Graphics.hpp"
#include <random>

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

struct UUIDComponent
{
	UUIDComponent() = delete;
	UUIDComponent(uint64_t elapsedTime)
	{
		static std::random_device rd;
		static std::mt19937_64 generator(rd());
		static std::uniform_int_distribution<uint64_t> uniformDistribution;

		uint64_t part1 = uniformDistribution(generator);
		uint64_t part2 = uniformDistribution(generator);

		m_uuid = (part1 << 64) | part2;
	}

	uint64_t m_uuid;
};
