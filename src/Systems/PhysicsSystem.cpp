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
        if (!level->HasComponent<MovementComponent>(player)) continue;
        if (!level->HasComponent<InputComponent>(player)) continue;

        InputComponent& inputComponent = level->GetComponent<InputComponent>(player);

        // Interpolation position if we are not the local player
        if (!level->IsEntityLocalPlayer(player))
        {
            InputArray& inputArray = level->GetComponent<InputArray>(player);
            if (!inputArray.m_inputs.empty())
            {
                inputComponent = inputArray.m_inputs[0];
                UpdateMovementComponent(player, level, inputComponent);
                sf::Vector2f offset = CalculatePhysics(player, level, deltaTime);
                MovementComponent& moveComp = level->GetComponent<MovementComponent>(player);

                if (moveComp.m_interpAlpha < 1.f)
                {
                    TransformComponent& transComp = level->GetComponent<TransformComponent>(player);

                    moveComp.m_interpolationTarget += offset;
                    sf::Vector2f lerped = Lerp<sf::Vector2f>(moveComp.m_startingInterpPosition, moveComp.m_interpolationTarget, moveComp.m_interpAlpha);
                    
                    if (lerped.y > 550.f)
                        lerped.y = 550.f;

                    moveComp.m_interpAlpha += deltaTime * 10.f;
                    transComp.m_x = lerped.x;
                    transComp.m_y = lerped.y;
                }
                else
                {
                    UpdateTransformComponent(player, level, offset);
                }


                if (inputArray.m_inputs.size() > 1)
                {
                    inputArray.m_inputs.erase(inputArray.m_inputs.begin());
                }
            }
            continue;
        }

        // If we are local player, just update position
        UpdateMovementComponent(player, level, inputComponent);
        sf::Vector2f offset = CalculatePhysics(player, level, deltaTime);
        UpdateTransformComponent(player, level, offset);
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
            ServerSocketComponent& serverSocketComponent = level->GetComponent<ServerSocketComponent>(NETWORK_ENTITY);
            for (TCPSocket& socket : serverSocketComponent.m_tcpSockets)
            {
                socket->send(packet);
            }
        }
    }
}

sf::Vector2f PhysicsSystem::ResimulatePhysics(Entity player, Level* level, float deltaTime)
{
    if (!level->HasComponent<InputArray>(player)) return sf::Vector2f(0.f, 0.f);

    InputArray& inputArray = level->GetComponent<InputArray>(player);

    if (inputArray.m_inputs.empty()) return sf::Vector2f(0.f, 0.f);
    
    // Here we use a hacky fix to make sure that when resimulating we have the appropriate player position
    TransformComponent& transComp = level->GetComponent<TransformComponent>(player);
    sf::Vector2f oldTransform(transComp.m_x, transComp.m_y);
    
    sf::Vector2f totalOffset = sf::Vector2f(0.f, 0.f);
    for (size_t i = 0; i < inputArray.m_inputs.size() - 1; ++i)
    {
        UpdateMovementComponent(player, level, inputArray.m_inputs[i]);
        sf::Vector2f offset = CalculatePhysics(player, level, deltaTime);
        totalOffset += offset;
        UpdateTransformComponent(player, level, offset);
    }

    InputComponent& inputComp = level->GetComponent<InputComponent>(player);
    inputComp = inputArray.m_inputs[inputArray.m_inputs.size() - 1];
    inputArray.m_inputs.clear();

    // Here we reset it back so that there is no jerky jumping when interpolating
    transComp.m_x = oldTransform.x;
    transComp.m_y = oldTransform.y;

    return totalOffset;
}

void PhysicsSystem::UpdateMovementComponent(Entity player, Level* level, InputComponent& inputComponent)
{
    MovementComponent& movementComp = level->GetComponent<MovementComponent>(player);
    movementComp.m_inputVelocity = sf::Vector2f(inputComponent.m_moveInput, inputComponent.m_jumpInput * movementComp.m_jumpStrength);
}

sf::Vector2f PhysicsSystem::CalculatePhysics(Entity player, Level* level, float deltaTime)
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

    sf::Vector2f toReturn = sf::Vector2f(0.f, 0.f);
    toReturn.x = movementComp.m_currentVelocity.x * deltaTime;
    toReturn.y = movementComp.m_currentVelocity.y * deltaTime;

    movementComp.m_inputVelocity = sf::Vector2f(0.f, 0.f);
    return toReturn;
}

void PhysicsSystem::UpdateTransformComponent(Entity player, Level* level, sf::Vector2f offset)
{
    TransformComponent& transformComp = level->GetComponent<TransformComponent>(player);
    transformComp.m_x += offset.x;
    transformComp.m_y += offset.y;
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
