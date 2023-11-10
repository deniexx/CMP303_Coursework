#pragma once

#include "ISystem.h"

class HitSystem : public ISystem
{
	
	void BeginSystem() override;
	void UpdateSystem(float deltaTime) override;
	void DestroySystem() override;

	void ClientUpdate() override;
	void ServerUpdate() override;
};

