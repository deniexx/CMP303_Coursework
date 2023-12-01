#pragma once

#include "ISystem.h"

struct TransformComponent;

class DeathSystem : public ISystem
{

public:
    
    void BeginSystem() override;
    void UpdateSystem(float deltaTime) override;
    void DestroySystem() override;

    float TimeBetweenUpdates() override;
    void SendUpdate() override;

    bool IsPlayerOutOfBounds(TransformComponent& transformComp);
};
