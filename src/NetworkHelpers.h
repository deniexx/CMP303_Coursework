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
};
