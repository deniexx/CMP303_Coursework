#include "PhysicsSystem.h"
#include "../Core/Application.h"
#include "../Messages/Messages.h"
#include "../Utilities/MathUtils.h"

void PhysicsSystem::BeginSystem()
{
    // @TODO: maybe something will be needed
}

void PhysicsSystem::UpdateSystem(float deltaTime)
{
    Level* level = Application::Instance->GetCurrentLevel().get();
    std::vector<Entity> players = level->GetAllPlayerEntities();
    
    for (Entity player : players)
    {
        // @TODO: Update this so that local players update every frame using the input component, while a remote player uses the InputArray
        // This might be the case if the player is currently being initialized, although it shouldn't really happen ever
        if (!level->HasComponent<MovementComponent>(player)) continue;

        if (!level->IsEntityLocalPlayer(player))
            ResimulatePhysics(player, level, deltaTime);
        
        if (!level->HasComponent<InputComponent>(player)) continue;
        InputComponent& inputComponent = level->GetComponent<InputComponent>(player);
        UpdateMovementComponent(player, level, inputComponent);
        CalculatePhysics(player, level, deltaTime);
    }
}

void PhysicsSystem::SendUpdate()
{
    Level* level = Application::Instance->GetCurrentLevel().get();
    if (level->IsServer())
    {
        bool update = false;
        const std::vector<Entity> players = level->GetAllPlayerEntities();
        LastPhysicsState& lps = level->GetComponent<LastPhysicsState>(NETWORK_ENTITY);

        sf::Packet packet;
        for (const Entity player : players)
        {
            TransformComponent& transComp = level->GetComponent<TransformComponent>(player);
            if (lps.m_state[player] == transComp)
            {
                lps.m_state[player] = transComp;
                continue; // skip player if no movement has happened since last physics update
            }

            update = true;
            PhysicsUpdateMessage message;
            message.m_playerID = player;
            message.m_x = transComp.m_x;
            message.m_y = transComp.m_y;

            packet << PHYSICSUPDATE_EVENTID;
            packet << message;
        }

        if (update)
        {
            const ServerSocketComponent& serverSocketComponent = level->GetComponent<ServerSocketComponent>(NETWORK_ENTITY);
            for (sf::TcpSocket* socket : serverSocketComponent.m_tcpSockets)
            {
                socket->send(packet);
            }
        }
    }
}

void PhysicsSystem::ResimulatePhysics(Entity player, Level* level, float deltaTime)
{
    if (!level->HasComponent<InputArray>(player)) return;

    InputArray& inputArray = level->GetComponent<InputArray>(player);

    if (inputArray.m_inputs.empty()) return;

    // @TODO: Do not just set the transform component after this, but save it as an interpolation target and interpolate to that position smoothly
    // @TODO: Update the calculate physics to only calculate physics and return a position vector
    // @TODO: Add another function that can be called to fill up the transform component with the newly found position variables
    for (size_t i = 0; i < inputArray.m_inputs.size() - 1; ++i)
    {
        UpdateMovementComponent(player, level, inputArray.m_inputs[i]);
        CalculatePhysics(player, level, deltaTime);
    }

    InputComponent& inputComp = level->GetComponent<InputComponent>(player);
    inputComp = inputArray.m_inputs[inputArray.m_inputs.size() - 1];
    inputArray.m_inputs.clear();
}

void PhysicsSystem::UpdateMovementComponent(Entity player, Level* level, InputComponent& inputComponent)
{
    MovementComponent& movementComp = level->GetComponent<MovementComponent>(player);
    movementComp.m_inputVelocity = sf::Vector2f(inputComponent.m_moveInput, inputComponent.m_jumpInput * movementComp.m_jumpStrength);
}

void PhysicsSystem::CalculatePhysics(Entity player, Level* level, float deltaTime)
{
    TransformComponent& transComp = level->GetComponent<TransformComponent>(player);
    MovementComponent& movementComp = level->GetComponent<MovementComponent>(player);

    float horizontalMovementMultiplier = movementComp.m_airControl;
    if (IsPlayerOnGround(transComp))
    {
        horizontalMovementMultiplier = 1;
        movementComp.m_currentVelocity.y -= movementComp.m_inputVelocity.y;
        movementComp.m_currentVelocity.x *= movementComp.m_lateralFriction;
    }

    if (!movementComp.m_impulseOverridesMovement)
    {
        movementComp.m_currentVelocity.x += (movementComp.m_inputVelocity.x * horizontalMovementMultiplier) * movementComp.m_acceleration;
        movementComp.m_currentVelocity.x = Clamp<float>(movementComp.m_currentVelocity.x, -movementComp.m_maxSpeed, movementComp.m_maxSpeed);
    }

    movementComp.m_currentVelocity.y += m_gravity;

    if (IsFalling(movementComp))
    {
        movementComp.m_impulseOverridesMovement = false;
        movementComp.m_currentVelocity.y *= movementComp.m_fallingSpeedMultiplier;
    }

    movementComp.m_currentVelocity += movementComp.m_impulseToBeApplied * deltaTime;
    movementComp.m_impulseToBeApplied = sf::Vector2f(0.f, 0.f);

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
