#include "DeathSystem.h"
#include "../Core/Application.h"
#include <vector>

#include "../NetworkHelpers.h"
#include "../Utilities/MathUtils.h"
#include "../Messages/Messages.h"


const float DeathDistance = 1300.f;

void DeathSystem::BeginSystem()
{
    
}

void DeathSystem::UpdateSystem(float deltaTime)
{
    Level* level = Application::Instance->GetCurrentLevel().get();

    // We handle this only on the server as this is a very important part of the game
    if (level->IsServer())
    {
        std::vector<Entity> players = level->GetAllPlayerEntities();
        std::vector<DeathEventMessage> messages;
        
        for (auto player : players)
        {
            TransformComponent& trans = level->GetComponent<TransformComponent>(player);
            if (IsPlayerOutOfBounds(trans))
            {
                if (level->GetLocalPlayerID() == player)
                {
                    Entity ent = level->CreateEntity();
                    TextComponent& text = level->EmplaceComponent<TextComponent>(ent);
                    text.text.setString("You died!");
                    text.text.setFont(Application::Instance->m_font);
                    text.text.setColor(sf::Color(255, 0, 0));
                    TransformComponent& t = level->GetComponent<TransformComponent>(ent);
                    t.m_x = (1280 / 2) - 64;
                    t.m_y = 180.f;
                }

                HitComponent& hitComp = level->GetComponent<HitComponent>(player);

                DeathEventMessage m;
                m.m_playerId = player;
                m.m_hitterId = hitComp.m_lastHitterId;

                messages.push_back(m);

                NetworkHelpers::KillEntity(player);
            }
        }

        if (messages.empty()) return;

        sf::Packet packet;
        for (auto& m : messages)
        {
            packet << DEATH_EVENTID;
            packet << m;
        }

        ServerSocketComponent& socketComp = level->GetComponent<ServerSocketComponent>(NETWORK_ENTITY);
        for (auto& socket : socketComp.m_tcpSockets)
        {
            socket->send(packet);
        }
    }
}

void DeathSystem::DestroySystem()
{
    
}

float DeathSystem::TimeBetweenUpdates()
{
    return 9999999.f;
}

void DeathSystem::SendUpdate()
{
    // @IGNORE: This we don't send updates from this system
}

bool DeathSystem::IsPlayerOutOfBounds(TransformComponent& transformComp)
{
    sf::Vector2u screenSize = Application::Instance->GetWindowSize();
    if (transformComp.m_x < -150.f || transformComp.m_x > screenSize.x + 150.f)
        return true;
    if (transformComp.m_y < -150.f)
        return true;
    
    return false;
}
