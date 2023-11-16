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

	static void UpdateInputForEntity(const InputUpdateMessage& message)
	{
		Level* level = Application::Instance->GetCurrentLevel().get();

		if (level->HasComponent<InputComponent>(message.m_playerID))
		{
			InputComponent& inputComp = level->GetComponent<InputComponent>(message.m_playerID);
			inputComp.m_moveInput = message.m_moveInput;
			inputComp.m_jumpInput = message.m_jumpInput;
			inputComp.m_attackInput = message.m_attackInput;
		}
	}
};
