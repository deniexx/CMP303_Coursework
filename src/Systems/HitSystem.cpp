#include "HitSystem.h"
#include "../Core/Application.h"
#include "../Utilities/MathUtils.h"
#include <cassert>

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
					MovementComponent& hitMoveComp = level->GetComponent<MovementComponent>(inPlayer);
					HitComponent& hitHitComp = level->GetComponent<HitComponent>(inPlayer);
					TransformComponent& hitTransComp = level->GetComponent<TransformComponent>(inPlayer);
					
					hitMoveComp.m_impulseOverridesMovement = true;
					sf::Vector2f hitPos(hitTransComp.m_x, hitTransComp.m_y);
					sf::Vector2f hitterPos(pTransComp.m_x, pTransComp.m_y);

					sf::Vector2f impulseDirection = hitPos - hitterPos;

					// Get length of vector
					float vectorLength = sqrt(impulseDirection.x * impulseDirection.x + impulseDirection.y * impulseDirection.y);
					
					// Normalize vector
					if (vectorLength > 0)
					{
						impulseDirection /= vectorLength;
					}
					else
					{
                        // We could not normalize velocity, check this out
						assert(false);
					}

					impulseDirection.y = -0.5f;
					impulseDirection.x = impulseDirection.x < 0 ? -0.5f : 0.5f;

					hitHitComp.m_damageMultiplier *= 1.1f;
					float impulseAmount = hitComp.m_impuseOnHit * hitHitComp.m_damageMultiplier;

					hitMoveComp.m_impulseToBeApplied = impulseDirection * impulseAmount;
				}
			}
		}
	}
}

void HitSystem::DestroySystem()
{

}
