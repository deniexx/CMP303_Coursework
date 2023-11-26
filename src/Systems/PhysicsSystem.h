#pragma once

#include "ISystem.h"
#include "../Core/CoreDefines.h"
#include "../Core/Level.h"

class PhysicsSystem : public ISystem
{
public:

    void BeginSystem() override;
    void UpdateSystem(float deltaTime) override;
    void DestroySystem() override;

    // We will send this update 10 times a second
    float TimeBetweenUpdates() override { return 0.5f; }
    void SendUpdate() override;

private:
    
    sf::Vector2f ResimulatePhysics(Entity player, Level* level, float deltaTime);
    
    void UpdateMovementComponent(Entity player, Level* level, InputComponent& inputComponent);
    sf::Vector2f CalculatePhysics(Entity player, Level* level, float deltaTime);
    void UpdateTransformComponent(Entity player, Level* level, sf::Vector2f offset);

    bool IsPlayerOnGround(const TransformComponent& transformComp);
    bool IsFalling(const MovementComponent& movementComp);

    
    float m_gravity = 9.81f;
};
