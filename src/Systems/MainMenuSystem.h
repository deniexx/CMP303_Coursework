#pragma once
#include "ISystem.h"

class MainMenuSystem : public ISystem
{
public:
    void BeginSystem() override;
    void UpdateSystem(float deltaTime) override;
    void DestroySystem() override;

    float TimeBetweenUpdates() override;
    void SendUpdate() override;
    
};
