#pragma once

#include <cstdint>
#include <string>
#include "../Components/Components.h"

// @NOTE: Every message must have an ID
static const uint8_t AUTHENTICATE_EVENTID = 255;
static const uint8_t NEWPLAYER_EVENTID = 0;
static const uint8_t PHYSICSUPDATE_EVENTID = 1;
static const uint8_t INPUTUPDATE_EVENTID = 2;
static const uint8_t HITREG_EVENTID = 3;
static const uint8_t DEATH_EVENTID = 4;
static const uint8_t FAILEDAUTHENTICATION_EVENTID = 254;

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

struct FailedAuthenticationMessage
{
    std::string reason;
};

inline sf::Packet& operator <<(sf::Packet& packet, const FailedAuthenticationMessage m)
{
    return packet << m.reason;
}

inline sf::Packet& operator >>(sf::Packet& packet, FailedAuthenticationMessage& m)
{
    return packet >> m.reason;
}
#pragma endregion 

#pragma region NewPlayer
struct NewPlayerMessage
{
    uint8_t m_playerID = -1;
    std::string m_playerName = std::string();
    sf::Int8 m_playerConnection = (sf::Int8)PlayerConnectionType::None;
    std::string m_fallbackAddress = std::string();
    sf::Color m_playerColor = sf::Color(0.f, 0.f, 0.f);

    float m_x = 0, m_y = 0;
};

inline sf::Packet& operator <<(sf::Packet& packet, const NewPlayerMessage& m)
{
    return packet << m.m_playerID << m.m_playerName << (sf::Int8)m.m_playerConnection << m.m_fallbackAddress << m.m_playerColor.r << m.m_playerColor.g << m.m_playerColor.b << m.m_x << m.m_y;
}

inline sf::Packet& operator >>(sf::Packet& packet, NewPlayerMessage& m)
{
    return packet >> m.m_playerID >> m.m_playerName >> m.m_playerConnection >> m.m_fallbackAddress >> m.m_playerColor.r >> m.m_playerColor.g >> m.m_playerColor.b >> m.m_x >> m.m_y;
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

inline sf::Packet& operator <<(sf::Packet& packet, const sf::Vector2f& m)
{
    return packet << m.x << m.y;
}

inline sf::Packet& operator >>(sf::Packet& packet, sf::Vector2f& m)
{
    return packet >> m.x >> m.y;
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

#pragma region HitReg
struct HitRegMessage
{
    uint8_t m_hitterId = -1;
    uint8_t m_hitId = -1;

    // hit location
    float m_hitX = 0, m_hitY = 0, m_hitterX = 0, m_hitterY = 0;
};

inline sf::Packet& operator <<(sf::Packet& packet, const HitRegMessage& m)
{
    return packet << m.m_hitId << m.m_hitterId << m.m_hitX << m.m_hitY << m.m_hitterX << m.m_hitterY;
}

inline sf::Packet& operator >>(sf::Packet& packet, HitRegMessage& m)
{
    return packet >> m.m_hitId >> m.m_hitterId >> m.m_hitX >> m.m_hitY >> m.m_hitterX >> m.m_hitterY;;
}
#pragma endregion

#pragma region Death
struct DeathEventMessage
{
    uint8_t m_playerId;
    uint8_t m_hitterId;
};

inline sf::Packet& operator <<(sf::Packet& packet, const DeathEventMessage& m)
{
    return packet << m.m_playerId << m.m_hitterId;
}

inline sf::Packet& operator >>(sf::Packet& packet, DeathEventMessage& m)
{
    return packet >> m.m_playerId >> m.m_hitterId;
}

#pragma endregion

// @TODO: Queue of packets and send them in case they need to arrive in order
// Map player IDs to queued packets, that way we can make sure each player receives their packages in order (only if we need to do so)
// We can use TCP for player input updates, if necessary, the server can try to update players with their position over UDP, hit visuals should also be over UDp
// Hit reg should use TCP
