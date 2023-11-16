#pragma once

#include "SFML/System/Vector2.hpp"
#include "ISystem.h"
#include <unordered_map>
#include "../Core/CoreDefines.h"
#include "../Core/Level.h"

struct PhysicsFrame
{
    double m_frameNum = 0;
    float m_x = 0;
    float m_y = 0;

    sf::Vector2f m_velocity;
};

class PhysicsSystem : public ISystem
{
public:

    void BeginSystem() override;
    void UpdateSystem(float deltaTime) override;
    void DestroySystem() override;

    // We will send this update 10 times a second
    float TimeBetweenUpdates() override { return 1.f; }
    void SendUpdate() override;

private:

    void UpdateMovementComponent(Entity player, Level* level, InputComponent& inputComponent);
    void CalculatePhysics(Entity player, Level* level, float deltaTime);

    bool IsPlayerOnGround(const TransformComponent& transformComp);
    bool IsFalling(const MovementComponent& movementComp);


    float m_gravity = 9.81f;
    std::unordered_map<Entity, PhysicsFrame> m_physicsFrames;
};