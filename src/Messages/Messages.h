#pragma once

#include <cstdint>
#include <string>
#include <SFML/Network/IpAddress.hpp>

// @NOTE: Every message must have an ID
static const uint8_t AUTHENTICATE_EVENTID = 255;
static const uint8_t NEWPLAYER_EVENTID = 0;
static const uint8_t PHYSICSUPDATE_EVENTID = 1;
static const uint8_t INPUTUPDATE_EVENTID = 2;

#pragma region Authentication
struct AuthenticationMessage
{
    std::string m_authenticationMessage;
    std::string m_playerName;
};

inline sf::Packet& operator <<(sf::Packet& packet, const AuthenticationMessage& m)
{
    return packet << m.m_authenticationMessage << m.m_playerName;
}

inline sf::Packet& operator >>(sf::Packet& packet, AuthenticationMessage& m)
{
    return packet >> m.m_authenticationMessage >> m.m_playerName;
}
#pragma endregion 

#pragma region NewPlayer
struct NewPlayerMessage
{
    uint8_t m_playerID = -1;
    std::string m_playerName = std::string();
    sf::Int8 m_playerConnection = (sf::Int8)PlayerConnectionType::None;
    std::string m_fallbackAddress = std::string();
};

inline sf::Packet& operator <<(sf::Packet& packet, const NewPlayerMessage& m)
{
    return packet << m.m_playerID << m.m_playerName << (sf::Int8)m.m_playerConnection << m.m_fallbackAddress;
}

inline sf::Packet& operator >>(sf::Packet& packet, NewPlayerMessage& m)
{
    return packet >> m.m_playerID >> m.m_playerName >> m.m_playerConnection >> m.m_fallbackAddress;
}
#pragma endregion 

#pragma region PhysicsUpdate
struct PhysicsUpdateMessage
{
    sf::Int8 m_playerID;
    float m_x, m_y;
};

inline sf::Packet& operator <<(sf::Packet& packet, const PhysicsUpdateMessage& m)
{
    return packet << m.m_playerID << m.m_x << m.m_y;
}

inline sf::Packet& operator >>(sf::Packet& packet, PhysicsUpdateMessage& m)
{
    return packet >> m.m_playerID >> m.m_x >> m.m_y;
}
#pragma endregion

#pragma region InputUpdate
inline sf::Packet& operator <<(sf::Packet& packet, const InputArray& m)
{
    packet << (uint32_t)m.m_inputs.size();
    for (size_t i = 0; i < m.m_inputs.size(); ++i)
    {
        packet << m.m_inputs[i].m_moveInput << m.m_inputs[i].m_jumpInput << m.m_inputs[i].m_attackInput << m.m_inputs[i].m_confirmInput << m.m_inputs[i].m_upDownNavigateInput;
    }

    return packet;
}

inline sf::Packet& operator >>(sf::Packet& packet, InputArray& m)
{
    uint32_t size;
    packet >> size;
    for (size_t i = 0; i < size; ++i)
    {
        InputComponent comp;
        packet >> comp.m_moveInput >> comp.m_jumpInput >> comp.m_attackInput >> comp.m_confirmInput >> comp.m_upDownNavigateInput;
        m.m_inputs.push_back(comp);
    }

    return packet;
}
#pragma endregion

// @TODO: Queue of packets and send them in case they need to arrive in order
// Map player IDs to queued packets, that way we can make sure each player receives their packages in order (only if we need to do so)
// We can use TCP for player input updates, if necessary, the server can try to update players with their position over UDP, hit visuals should also be over UDp
// Hit reg should use TCP