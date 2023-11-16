#pragma once

#include "../Core/CoreDefines.h"
#include "ISystem.h"

#include "SFML/Window/Keyboard.hpp"

class InputSystem : public ISystem
{
	void BeginSystem() override;
	void UpdateSystem(float deltaTime) override;
	void DestroySystem() override;

	float TimeBetweenUpdates() override { return 0.5f; }
	void SendUpdate() override;
};
