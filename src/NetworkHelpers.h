#pragma once

#include "Core/Application.h"
#include "Messages/Messages.h"

namespace NetworkHelpers
{
    static void UpdatePhysicsForEntity(const PhysicsUpdateMessage& message)
    {
        Level* level = Application::Instance->GetCurrentLevel().get();

        if (level->HasComponent<TransformComponent>(message.m_playerID))
		{
			TransformComponent& transComp = level->GetComponent<TransformComponent>(message.m_playerID);
			transComp.m_x = message.m_x;
			transComp.m_y = message.m_y;
		}
    }
};
