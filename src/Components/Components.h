#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <random>

struct TransformComponent
{
	TransformComponent() = default;
	TransformComponent(TransformComponent& other) = default;
	TransformComponent(float x, float y) : m_x(x), m_y(y) {}

	bool operator==(const TransformComponent& other) const
	{
		return m_x == other.m_x && m_y == other.m_y;
	}

	float m_x = 0, m_y = 0;
};

struct SpriteComponent
{
	SpriteComponent() = default;
	SpriteComponent(SpriteComponent& other) = default;
	SpriteComponent(sf::Sprite sprite) : m_sprite(std::move(sprite)) {}
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
	TagComponent(TagComponent&  other) = default;
	TagComponent(std::string tag) : m_tag(std::move(tag)) {}

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

struct NetworkPlayerComponent
{
	NetworkPlayerComponent() = delete;
	NetworkPlayerComponent(NetworkPlayerComponent& other) = delete;
	NetworkPlayerComponent(PlayerConnectionType connectionType) : m_connectionType(connectionType) {}

	PlayerConnectionType m_connectionType = PlayerConnectionType::None;
};

struct InputComponent
{
	InputComponent() = default;

	int32_t m_moveInput = 0;
	int32_t m_upDownNavigateInput = 0;
	int32_t m_jumpInput = 0;
	int32_t m_attackInput = 0;
	int32_t m_confirmInput = 0;
};

struct DeleteComponent
{
	bool m_markedForDelete = false;
};

struct InputArray
{
	std::vector<InputComponent> m_inputs;
};

struct LastPhysicsState
{
	std::unordered_map<Entity, TransformComponent> m_state;
};

struct MovementComponent
{
	MovementComponent() = default;

	sf::Vector2f m_inputVelocity = sf::Vector2f(0.f, 0.f);
	sf::Vector2f m_impulseToBeApplied = sf::Vector2f(0.f, 0.f);
	sf::Vector2f m_currentVelocity = sf::Vector2f(0.f, 0.f);
	sf::Vector2f m_interpolationTarget = sf::Vector2f(0.f, 0.f);
	sf::Vector2f m_startingInterpPosition = sf::Vector2f(0.f, 0.f);
	
	float m_interpAlpha = 0.f;
	
    sf::Vector2f m_lastPositionBeforeNetUpdate = sf::Vector2f(0.f, 0.f);
	float m_maxSpeed = 300;
	float m_acceleration = 150;
	float m_velocityInterp = 0;
	float m_jumpStrength = 250;
	float m_lateralFriction = 0.75f;
	float m_airControl = 0.4f;
	float m_fallingSpeedMultiplier = 1.f;
	bool m_impulseOverridesMovement = false;
};

struct HitComponent
{
	HitComponent() = default;

	float m_damageMultiplier = 1.f;
	float m_impuseOnHit = 550.f;
	sf::Vector2f m_attackSize = sf::Vector2f(48.f, 48.f);
	double m_lastAttackTime = 0.f;
	float m_attackCooldown = 0.15f;

	uint8_t m_lastHitterId = -1;
};

struct TCPSocket
{
	sf::TcpSocket* m_tcpSocket;
	bool m_waitingForResponse = false;
	uint64_t m_lastUpdateTime = 0;

	sf::TcpSocket* operator->()
	{
		return m_tcpSocket;
	}

	sf::TcpSocket& operator*()
	{
		return *m_tcpSocket;
	}

	bool operator==(TCPSocket& other)
	{
		return m_tcpSocket == other.m_tcpSocket;
	}

	bool operator==(const TCPSocket& other)
	{
		return m_tcpSocket == other.m_tcpSocket;
	}

	bool operator<(TCPSocket& other)
	{
		return m_tcpSocket < other.m_tcpSocket;
	}
};

struct ServerSocketComponent
{
	sf::TcpListener m_tcpListener;
	sf::SocketSelector m_socketSelector;
	std::vector<TCPSocket> m_tcpSockets;
	sf::UdpSocket m_udpSocket;
};

struct ClientSocketComponent
{
	sf::TcpSocket m_tcpSocket;
	sf::UdpSocket m_udpSocket;
};

struct TextComponent
{
	sf::Text text;
	sf::Vector2f offset = sf::Vector2f(0.f, 0.f);
};
