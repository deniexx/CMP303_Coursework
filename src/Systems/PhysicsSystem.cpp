#include "PhysicsSystem.h"
#include "../Core/Application.h"
#include "../Utilities/MathUtils.h"

#include <iostream>

void PhysicsSystem::BeginSystem()
{
    // @TODO: maybe something will be needed
}

void PhysicsSystem::UpdateSystem(float deltaTime)
{
    std::shared_ptr<Level> level = Application::Instance->GetCurrentLevel();
    std::vector<Entity> players = level->GetAllPlayerEntities();
    
    for (const auto& player : players)
    {
        // This might be the case if the player is currently being initialized, although it shouldn't really happen ever
        if (!level->HasComponent<MovementComponent>(player)) continue;
        if (!level->HasComponent<InputComponent>(player)) continue;

        UpdateMovementComponent(player, level);
        CalculatePhysics(player, level, deltaTime);
    }

    CheckForCollision(players);
}

void PhysicsSystem::UpdateMovementComponent(Entity player, std::shared_ptr<Level> level)
{
    InputComponent& inputComp = level->GetComponent<InputComponent>(player);
    MovementComponent& movementComp = level->GetComponent<MovementComponent>(player);

    movementComp.m_inputVelocity = sf::Vector2f(inputComp.m_moveInput, inputComp.m_jumpInput * movementComp.m_jumpStrength);
    inputComp.m_jumpInput = 0;
    inputComp.m_moveInput = 0;
}

void PhysicsSystem::CalculatePhysics(Entity player, std::shared_ptr<Level> level, float deltaTime)
{
    TransformComponent& transComp = level->GetComponent<TransformComponent>(player);
    MovementComponent& movementComp = level->GetComponent<MovementComponent>(player);

    float horizontalMovementMultiplier = movementComp.m_airControl;
    if (IsPlayerOnGround(transComp))
    {
        horizontalMovementMultiplier = 1;
        movementComp.m_currentVelocity.y -= movementComp.m_inputVelocity.y;
    }

	movementComp.m_currentVelocity.x += movementComp.m_inputVelocity.x * movementComp.m_acceleration * horizontalMovementMultiplier;
	movementComp.m_currentVelocity.x /= movementComp.m_lateralFriction;
	movementComp.m_currentVelocity.x = Clamp<float>(movementComp.m_currentVelocity.x, -movementComp.m_maxSpeed, movementComp.m_maxSpeed);

    movementComp.m_currentVelocity.y += m_gravity;

    if (IsFalling(movementComp))
    {
        movementComp.m_currentVelocity.y *= movementComp.m_fallingSpeedMultiplier;
    }


    transComp.m_x += movementComp.m_currentVelocity.x * deltaTime;
    transComp.m_y += movementComp.m_currentVelocity.y * deltaTime;

    if (abs(movementComp.m_currentVelocity.x) < 0.0001f)
        movementComp.m_currentVelocity.x = 0.f;
    if (abs(movementComp.m_currentVelocity.y) < 0.0001f)
        movementComp.m_currentVelocity.y = 0.f;

    // @TODO: Maybe change how this works, we'll figure it out for now just do a solid check
    if (transComp.m_y > 550.f)
    {
        transComp.m_y = 550.f;
        movementComp.m_currentVelocity.y = 0.f;
    }

    movementComp.m_inputVelocity = sf::Vector2f(0.f, 0.f);
}

void PhysicsSystem::CheckForCollision(std::vector<Entity>& players)
{

}

bool PhysicsSystem::IsPlayerOnGround(const TransformComponent& transformComp)
{
    return transformComp.m_y > 545.f;
}

bool PhysicsSystem::IsFalling(const MovementComponent& movementComp)
{
    return movementComp.m_currentVelocity.y > 0;
}

void PhysicsSystem::DestroySystem()
{

}

void PhysicsSystem::ClientUpdate()
{

}

void PhysicsSystem::ServerUpdate()
{

}
