#pragma once

#include "Core/Application.h"
#include "Messages/Messages.h"

namespace NetworkHelpers
{
    static void UpdatePhysicsForEntity(const PhysicsUpdateMessage& message)
    {
    	return;
        Level* level = Application::Instance->GetCurrentLevel().get();

        if (level->HasComponent<TransformComponent>(message.m_playerID))
		{
			//MovementComponent& movementComp = level->GetComponent<MovementComponent>(message.m_playerID);
			//movementComp.m_interpolationTarget.x = message.m_x;
			//movementComp.m_interpolationTarget.y = message.m_y;
		}
    }

    static bool ApplyHit(Entity hitter, Entity hit, TransformComponent& hitterTransComp, TransformComponent& hitTransComp)
    {
		Level* level = Application::Instance->GetCurrentLevel().get();

		MovementComponent& hitMoveComp = level->GetComponent<MovementComponent>(hit);
		HitComponent& hitHitComp = level->GetComponent<HitComponent>(hit);
		HitComponent& hitterHitComp = level->GetComponent<HitComponent>(hitter);

		hitMoveComp.m_impulseOverridesMovement = true;
		sf::Vector2f hitPos(hitTransComp.m_x, hitTransComp.m_y);
		sf::Vector2f hitterPos(hitterTransComp.m_x, hitterTransComp.m_y);

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
			return false;
		}

		impulseDirection.y = -0.5f;
		impulseDirection.x = impulseDirection.x < 0 ? -0.5f : 0.5f;

		hitHitComp.m_damageMultiplier *= 1.1f;
		float impulseAmount = hitterHitComp.m_impuseOnHit * hitHitComp.m_damageMultiplier;

		hitMoveComp.m_impulseToBeApplied = impulseDirection * impulseAmount;
		return true;
    }
};
