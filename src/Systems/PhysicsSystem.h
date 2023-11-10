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

    virtual void BeginSystem() override;
    virtual void UpdateSystem(float deltaTime) override;
    virtual void DestroySystem() override;

    virtual void ClientUpdate() override;
    virtual void ServerUpdate() override;

private:

    void UpdateMovementComponent(Entity player, std::shared_ptr<Level> level);
    void CalculatePhysics(Entity player, std::shared_ptr<Level> level, float deltaTime);
    void CheckForCollision(std::vector<Entity>& players);

    bool IsPlayerOnGround(const TransformComponent& transformComp);
    bool IsFalling(const MovementComponent& movementComp);


    float m_gravity = 9.81f;
    std::unordered_map<Entity, PhysicsFrame> m_physicsFrames;
};