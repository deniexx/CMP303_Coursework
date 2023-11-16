#pragma once

class ISystem
{

public:

    virtual ~ISystem() = default;
    virtual void BeginSystem() = 0;
    virtual void UpdateSystem(float deltaTime) = 0;
    virtual void DestroySystem() = 0;

    virtual float TimeBetweenUpdates() = 0;
    virtual void SendUpdate() = 0;
};
