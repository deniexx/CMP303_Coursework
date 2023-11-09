#pragma once

#include "../Core/CoreDefines.h"
#include "ISystem.h"

#include "SFML/Window/Keyboard.hpp"

class InputSystem : public ISystem
{
	virtual void BeginSystem();
	virtual void UpdateSystem(float deltaTime);
	virtual void DestroySystem();

	virtual void ClientUpdate();
	virtual void ServerUpdate();
};
