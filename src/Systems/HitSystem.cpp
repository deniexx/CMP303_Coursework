#include "HitSystem.h"
#include "../Core/Application.h"
#include <cassert>
#include "../Messages/Messages.h"
#include "../NetworkHelpers.h"

void HitSystem::BeginSystem()
{

}

void HitSystem::UpdateSystem(float deltaTime)
{
	Level* level = Application::Instance->GetCurrentLevel().get();
	std::vector<Entity> players = level->GetAllPlayerEntities();

	for (const auto& player : players)
	{
		if (!level->HasComponent<InputComponent>(player)) continue;
		if (!level->HasComponent<HitComponent>(player)) continue;
		if (!level->IsEntityLocalPlayer(player)) continue;
        if (!level->IsEntityLocalPlayer(player)) continue;

		InputComponent& inputComp = level->GetComponent<InputComponent>(player);
		HitComponent& hitComp = level->GetComponent<HitComponent>(player);

		if (inputComp.m_attackInput > 0 && hitComp.m_lastAttackTime + hitComp.m_attackCooldown < level->GetElapsedTime())
		{
			inputComp.m_attackInput = 0;
			hitComp.m_lastAttackTime = level->GetElapsedTime();

			TransformComponent& pTransComp = level->GetComponent<TransformComponent>(player);
			sf::FloatRect hitRect(sf::Vector2f(pTransComp.m_x, pTransComp.m_y), hitComp.m_attackSize);

			for (const auto& inPlayer : players)
			{
				if (player == inPlayer) continue;

				SpriteComponent& spriteComp = level->GetComponent<SpriteComponent>(inPlayer);

				if (hitRect.intersects(spriteComp.m_sprite.getGlobalBounds()))
				{
					TransformComponent& hitTransComp = level->GetComponent<TransformComponent>(inPlayer);

					if (!NetworkHelpers::ApplyHit(player, inPlayer, pTransComp, hitTransComp, inPlayer)) assert(false);
									
					if (level->IsServer())
					{
						// Redistribute hit packages
                        sf::Packet packet;
                        HitRegMessage message;
                        message.m_hitId = inPlayer;
                        message.m_hitterId = player;
                        message.m_hitX = hitTransComp.m_x;
                        message.m_hitY = hitTransComp.m_y;
                        message.m_hitterX = pTransComp.m_x;
                        message.m_hitterY = pTransComp.m_y;

                        packet << HITREG_EVENTID;
                        packet << message;

                        ServerSocketComponent& socketComp = level->GetComponent<ServerSocketComponent>(NETWORK_ENTITY);

						for (sf::TcpSocket* socket : socketComp.m_tcpSockets)
						{
							socket->send(packet);
						}
					}
					else
					{
						sf::Packet packet;
						HitRegMessage message;
						message.m_hitId = inPlayer;
						message.m_hitterId = player;
						message.m_hitX = hitTransComp.m_x;
						message.m_hitY = hitTransComp.m_y;
						message.m_hitterX = pTransComp.m_x;
						message.m_hitterY = pTransComp.m_y;
						packet << HITREG_EVENTID;
						packet << message;

						ClientSocketComponent& socketComp = level->GetComponent<ClientSocketComponent>(NETWORK_ENTITY);
						socketComp.m_tcpSocket.send(packet);
					}
				}
			}
		}
	}
}

void HitSystem::DestroySystem()
{

}
