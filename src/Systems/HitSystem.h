#pragma once

#include "ISystem.h"

class HitSystem : public ISystem
{
	
	void BeginSystem() override;
	void UpdateSystem(float deltaTime) override;
	void DestroySystem() override;

	float TimeBetweenUpdates() override { return 99999999.f; }
	void SendUpdate() override {}
};

