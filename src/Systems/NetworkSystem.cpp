#include "NetworkSystem.h"

#include <ctime>
#include <fstream>

#include "SFML/System/Time.hpp"

#include <iostream>

#include "../Utilities/MathUtils.h"
#include "../NetworkHelpers.h"
#include "../Core/Application.h"
#include "../Messages/Messages.h"

void NetworkSystem::BeginSystem()
{
    if (Application::Instance->GetCurrentLevel()->IsServer())
    {
        ServerSocketComponent& serverSocketComponent = GetServerSocketComponent();
    
        serverSocketComponent.m_tcpListener.listen(28000);
        serverSocketComponent.m_tcpListener.setBlocking(false);
        serverSocketComponent.m_udpSocket.setBlocking(false);
    }
    else
    {
        ClientSocketComponent& clientSocketComponent = GetClientSocketComponent();
        clientSocketComponent.m_tcpSocket.setBlocking(false);
        clientSocketComponent.m_udpSocket.setBlocking(false);

        std::ifstream file("assets/connectionIp.txt");
        std::string line;
        getline(file, line);
        clientSocketComponent.m_tcpSocket.connect(line, 28000, sf::seconds(5));
    }
}

void NetworkSystem::UpdateSystem(float deltaTime)
{
    Level* level = Application::Instance->GetCurrentLevel().get();
    
    if (level->IsServer())
    {
        ServerUpdate(deltaTime);
    }
    else
    {
        ClientUpdate();
    }
}

void NetworkSystem::DestroySystem()
{
    
}

void NetworkSystem::ServerUpdate(float deltaTime)
{
    ServerSocketComponent& serverSocketComponent = GetServerSocketComponent();
    ServerCheckConnections(serverSocketComponent, deltaTime);
    ServerAcceptConnections(serverSocketComponent);
    ServerReceivePackets(serverSocketComponent);
}

ServerSocketComponent& NetworkSystem::GetServerSocketComponent()
{
    return Application::Instance->GetCurrentLevel()->GetComponent<ServerSocketComponent>(NETWORK_ENTITY);
}

ClientSocketComponent& NetworkSystem::GetClientSocketComponent()
{
    return Application::Instance->GetCurrentLevel()->GetComponent<ClientSocketComponent>(NETWORK_ENTITY);
}

void NetworkSystem::RedistributePacket(ServerSocketComponent& socketComponent, TCPSocket& client, sf::Packet& packet)
{
    for (auto& socket : socketComponent.m_tcpSockets)
    {
        if (socket == client)
            continue;
        
        socket->send(packet);
    }
}

void NetworkSystem::ServerCheckConnections(ServerSocketComponent& socketComponent, float deltaTime)
{
    Level* level = Application::Instance->GetCurrentLevel().get();
    int size = socketComponent.m_tcpSockets.size();

    for (int i = size; i > 0; --i)
    {
        if (i < socketComponent.m_tcpSockets.size() && socketComponent.m_tcpSockets[i].m_waitingForResponse)
        {
            socketComponent.m_tcpSockets[i].m_waitingForResponse = false;
            if (level->GetGameTime() - socketComponent.m_tcpSockets[i].m_lastUpdateTime > MAX_PING_ALLOWED)
            {
                sf::Packet packet;
                packet << ERROR_EVENTID;
                std::string errorMessage = "More ping than allowed!";
                packet << errorMessage;
                socketComponent.m_tcpSockets[i]->send(packet);

                sf::Packet playerLeft;
                playerLeft << PLAYERLEFT_EVENTID;
                uint8_t p = i + 2;
                playerLeft << p;

                RedistributePacket(socketComponent, socketComponent.m_tcpSockets[i], playerLeft);
                level->GetComponent<DeleteComponent>(i + 2).m_markedForDelete = true;
            }
        }
    }
}

void NetworkSystem::ServerAcceptConnections(ServerSocketComponent& socketComponent)
{
    sf::TcpSocket* tcpSocket = new sf::TcpSocket;
    if (socketComponent.m_tcpListener.accept(*tcpSocket) == sf::Socket::Done)
    {

        if (socketComponent.m_tcpSockets.size() < 6)
        {
			sf::Packet packet;
			packet << AUTHENTICATE_EVENTID;
			packet << AUTHENTICATION_MESSAGE_SERVER;
			tcpSocket->send(packet);

			TCPSocket socket;
            socket.m_waitingForResponse = false;
			socket.m_tcpSocket = tcpSocket;
			socketComponent.m_socketSelector.add(*tcpSocket);
			socketComponent.m_tcpSockets.push_back(socket);
        }
        else
        {
            sf::Packet error;
            error << ERROR_EVENTID;
            error << "Max players reached!";
            tcpSocket->send(error);

            delete tcpSocket;
        }
    }
}

void NetworkSystem::ServerReceivePackets(ServerSocketComponent& socketComponent)
{
    if (socketComponent.m_socketSelector.wait(sf::milliseconds(1)))
    {
        for (auto& client : socketComponent.m_tcpSockets)
        {
            if (socketComponent.m_socketSelector.isReady(*client))
            {
                sf::Packet packet;
                if (client->receive(packet) == sf::Socket::Done)
                {
                    uint8_t eventID;
                    packet >> eventID;
                    std::cout << NetworkHelpers::EventToString(eventID);

                    switch (eventID)
                    {
                    case AUTHENTICATE_EVENTID:
                        {
                            AuthenticationMessage message;
                            packet >> message;
                            ServerCheckAuthentication(socketComponent, client, message);
                        }
                        break;
                    case PHYSICSUPDATE_EVENTID:
                        {
                            PhysicsUpdateMessage message;
                            packet >> message;
                            NetworkHelpers::UpdatePhysicsForEntity(message);
                        }
                        break;
					case INPUTUPDATE_EVENTID:
					    {
					        ServerUpdateInputArrays(socketComponent, client, packet);
					    }
					    break;
                    case HITREG_EVENTID:
                        {
                            ServerUpdateHitReg(socketComponent, client, packet);
                        }
                        break;
                    case PING_EVENTID:
                        {
                            ServerHandlePingEvent(socketComponent, client, packet);
                        }
                        break;
                    case PLAYERLEFT_EVENTID:
					    {
                            uint8_t id;
                            packet >> id;
                            RedistributePacket(socketComponent, client, packet);
                            Application::Instance->GetCurrentLevel()->GetComponent<DeleteComponent>(id).m_markedForDelete = true;
					    }
                        break;
                    }
                }
            }
        }
    }
}

void NetworkSystem::ServerCheckAuthentication(ServerSocketComponent& socketComponent, TCPSocket& socket, const AuthenticationMessage& message)
{
    if (message.m_authenticationMessage == AUTHENTICATION_MESSAGE_CLIENT)
    {
        Level* level = Application::Instance->GetCurrentLevel().get();
        // Server authenticated client successfully, send a message for successful connection to all clients
        std::vector<Entity> players = level->GetAllPlayerEntities();
        const Entity playerEntity = Application::Instance->GetCurrentLevel()->CreatePlayer(-1, message.m_playerName);

        for (auto& client : socketComponent.m_tcpSockets)
        {
            // Alerts all players to create a character for the newly joined person
            sf::Packet packet;
            packet << NEWPLAYER_EVENTID;
        
            NewPlayerMessage newPlayerMessage;
            newPlayerMessage.m_playerID = playerEntity;
            newPlayerMessage.m_playerName = message.m_playerName;
            newPlayerMessage.m_playerColor = level->GetComponent<SpriteComponent>(playerEntity).m_sprite.getColor();
            newPlayerMessage.m_playerConnection = client == socket ? (sf::Int8)PlayerConnectionType::ClientLocal : (sf::Int8)PlayerConnectionType::ClientRemote;
            packet << newPlayerMessage;
            
            client->send(packet);
        }

        // Tell the new player to create entities for all the other players
        sf::Packet packet;
        for (Entity player : players)
        {
            TransformComponent& comp = level->GetComponent<TransformComponent>(player);
            
            packet << NEWPLAYER_EVENTID;
            
            NewPlayerMessage newPlayerMessage;
            newPlayerMessage.m_playerID = player;
            newPlayerMessage.m_playerName = level->GetComponent<TagComponent>(player).m_tag;
            newPlayerMessage.m_playerConnection = (sf::Int8)PlayerConnectionType::ClientRemote;
            newPlayerMessage.m_playerColor = level->GetComponent<SpriteComponent>(player).m_sprite.getColor();
            newPlayerMessage.m_x = comp.m_x;
            newPlayerMessage.m_y = comp.m_y;

            packet << newPlayerMessage;
        }

        socket->send(packet);
    }
    else
    {
        FailedAuthenticationMessage failedAuthMessage;
        failedAuthMessage.reason = "Server was not able to authenticate user!";

        sf::Packet packet;
        packet << FAILEDAUTHENTICATION_EVENTID;
        packet << failedAuthMessage;
        socket->send(packet);
        socket->disconnect();
        socketComponent.m_socketSelector.remove(*socket);
        std::erase(socketComponent.m_tcpSockets, socket);
    }
}

void NetworkSystem::ServerUpdateInputArrays(ServerSocketComponent& socketComponent, TCPSocket& client, sf::Packet& packet)
{
    Level* level = Application::Instance->GetCurrentLevel().get();
    std::vector<Entity> players = level->GetAllPlayerEntities();

    uint8_t playerId;
    packet >> playerId;

	bool found = false;
	for (Entity player : players)
	{
		if (player == playerId)
		{
			found = true;
			break;
		}
	}

	sf::Vector2f lastSavedPos;
	if (!found)
	{
		InputArray inputArray;
		packet >> inputArray;
		packet >> lastSavedPos;

		return;
	}

	InputArray& inputArray = level->GetComponent<InputArray>(playerId);
	packet >> inputArray;
	packet >> lastSavedPos;

    MovementComponent& moveComp = level->GetComponent<MovementComponent>(playerId);

    // If the delta in position is more than the acceptable amount, fix it up
    if (Length(moveComp.m_lastPositionBeforeNetUpdate - lastSavedPos) > ACCEPTABLE_POSITION_DELTA)
    {
        MovementComponent& moveComp = level->GetComponent<MovementComponent>(playerId);
        TransformComponent& transComp = level->GetComponent<TransformComponent>(playerId);
        moveComp.m_interpolationTarget = lastSavedPos;
        moveComp.m_interpAlpha = 0.f;
        moveComp.m_startingInterpPosition = sf::Vector2f(transComp.m_x, transComp.m_y);
    }

    moveComp.m_lastPositionBeforeNetUpdate = lastSavedPos;

    RedistributePacket(socketComponent, client, packet);
}

void NetworkSystem::ServerUpdateHitReg(ServerSocketComponent& socketComponent, TCPSocket& client, sf::Packet& packet)
{
    HitRegMessage message;
    packet >> message;

    TransformComponent hitterTrans(message.m_hitterX, message.m_hitterY);
    TransformComponent hitTrans(message.m_hitX, message.m_hitY);
    NetworkHelpers::ApplyHit(message.m_hitterId, message.m_hitId, hitterTrans, hitTrans, message.m_hitterId);
    RedistributePacket(socketComponent, client, packet);
}

void NetworkSystem::ServerHandlePingEvent(ServerSocketComponent& socketComponent, TCPSocket& client, sf::Packet& packet)
{
    Level* level = Application::Instance->GetCurrentLevel().get();

	int i;
	packet >> i;

	socketComponent.m_tcpSockets[i].m_waitingForResponse = false;
    float gameTime = socketComponent.m_tcpSockets[i].m_lastUpdateTime;

    if (level->GetGameTime() - gameTime > MAX_PING_ALLOWED)
    {
		sf::Packet packet;
		packet << ERROR_EVENTID;
		std::string errorMessage = "More ping than allowed!";
		packet << errorMessage;
		socketComponent.m_tcpSockets[i]->send(packet);

		sf::Packet playerLeft;
		playerLeft << PLAYERLEFT_EVENTID;
        uint8_t p = i + 2;
		playerLeft << p;

		RedistributePacket(socketComponent, client, playerLeft);
        level->GetComponent<DeleteComponent>(i + 2).m_markedForDelete = true;
    }
}

void NetworkSystem::ClientUpdate()
{
    ClientSocketComponent& clientSocketComponent = GetClientSocketComponent();
    ClientReceivePackets(clientSocketComponent);
}

void NetworkSystem::ClientReceivePackets(ClientSocketComponent& socketComponent)
{
    sf::Packet packet;
    if (socketComponent.m_tcpSocket.receive(packet) != sf::Socket::Done)
    {
        return;
    }

	while (!packet.endOfPacket())
	{
        uint8_t eventID;
        packet >> eventID;
        std::cout << NetworkHelpers::EventToString(eventID);

        switch (eventID)
        {
        case AUTHENTICATE_EVENTID:
            {
                std::string authenticationMessage;
                packet >> authenticationMessage;
                ClientCheckAuthentication(socketComponent, authenticationMessage);
            }
            break;
        case NEWPLAYER_EVENTID:
            {
                NewPlayerMessage message;
                packet >> message;
                ClientNewPlayerEvent(socketComponent, message);
            }
            break;
        case PHYSICSUPDATE_EVENTID:
            {
                PhysicsUpdateMessage message;
                packet >> message;
                NetworkHelpers::UpdatePhysicsForEntity(message);
            }
            break;
        case INPUTUPDATE_EVENTID:
            {
                ClientProcessInputReceived(socketComponent, packet);
            }
            break;
        case FAILEDAUTHENTICATION_EVENTID:
            {
                ClientProcessFailedAuthentication(socketComponent, packet);
            }
            break;
        case HITREG_EVENTID:
            {
                HitRegMessage message;
                packet >> message;
                TransformComponent hitterTrans(message.m_hitterX, message.m_hitterY);
                TransformComponent hitTrans(message.m_hitX, message.m_hitY);
                NetworkHelpers::ApplyHit(message.m_hitterId, message.m_hitId, hitterTrans, hitTrans, message.m_hitterId);
            }
            break;
        case DEATH_EVENTID:
            {
                DeathEventMessage message;
                packet >> message;
                ClientHandleDeathEvent(socketComponent, message);
            }
            break;
        case PING_EVENTID:
            {
                int i;
                float time;
                packet >> i >> time;
                socketComponent.m_tcpSocket.send(packet);
            }
            break;
        case ERROR_EVENTID:
            {
                Application::Instance->m_errorMessage = "";
                packet >> Application::Instance->m_errorMessage;
                Application::Instance->SwitchLevel(GameLevel::MainMenu);
            }
            break;
        case PLAYERLEFT_EVENTID:
            {
                uint8_t i;
                packet >> i;
                Application::Instance->GetCurrentLevel()->DestroyEntity(i);
            }
            break;
        }
    }
}

void NetworkSystem::ClientCheckAuthentication(ClientSocketComponent& socketComponent, const std::string& authenticationMessage)
{
    if (authenticationMessage == AUTHENTICATION_MESSAGE_SERVER)
    {
        // Client authenticated server successfully
        sf::Packet packet;
        packet << AUTHENTICATE_EVENTID;

        AuthenticationMessage message;
        message.m_authenticationMessage = AUTHENTICATION_MESSAGE_CLIENT;
        message.m_playerName = "PlayerName";
        packet << message;
        socketComponent.m_tcpSocket.send(packet);
    }
    else
    {
        socketComponent.m_tcpSocket.disconnect();
        Application::Instance->m_errorMessage = "Failed Authentication!";
        Application::Instance->SwitchLevel(GameLevel::MainMenu);
    }
}

void NetworkSystem::ClientNewPlayerEvent(ClientSocketComponent& socketComponent, const NewPlayerMessage& message)
{
    Level* level = Application::Instance->GetCurrentLevel().get();
    const bool isLocal = message.m_playerConnection == (sf::Int8)PlayerConnectionType::ClientLocal;
    uint8_t playerId = level->CreatePlayer(message.m_playerID, message.m_playerName, isLocal);

    TransformComponent& transComp = level->GetComponent<TransformComponent>(playerId);
    transComp.m_x = message.m_x;
    transComp.m_y = message.m_y;

    level->GetComponent<SpriteComponent>(playerId).m_sprite.setColor(message.m_playerColor);
}

void NetworkSystem::ClientProcessInputReceived(ClientSocketComponent& socketComponent, sf::Packet& packet)
{
    Level* level = Application::Instance->GetCurrentLevel().get();
    std::vector<Entity> players = level->GetAllPlayerEntities();

    uint8_t playerId;
    packet >> playerId;

    bool found = false;
    for (Entity player : players)
    {
        if (player == playerId)
        {
            found = true;
            break;
        }
    }

    sf::Vector2f lastSavedPos;
    if (!found)
    {
        InputArray inputArray;
		packet >> inputArray;

        packet >> lastSavedPos;
        return;
    }

    InputArray& inputArray = level->GetComponent<InputArray>(playerId);
    packet >> inputArray;
    packet >> lastSavedPos;
    
    MovementComponent& moveComp = level->GetComponent<MovementComponent>(playerId);

    if (Length(moveComp.m_lastPositionBeforeNetUpdate - lastSavedPos) > ACCEPTABLE_POSITION_DELTA)
    {
        MovementComponent& moveComp = level->GetComponent<MovementComponent>(playerId);
        TransformComponent& transComp = level->GetComponent<TransformComponent>(playerId);
        moveComp.m_interpolationTarget = lastSavedPos;
        moveComp.m_interpAlpha = 0.f;
        moveComp.m_startingInterpPosition = sf::Vector2f(transComp.m_x, transComp.m_y);
    }

    moveComp.m_lastPositionBeforeNetUpdate = lastSavedPos;
}

void NetworkSystem::ClientProcessFailedAuthentication(ClientSocketComponent& socketComponent, sf::Packet& packet)
{
    FailedAuthenticationMessage message;
    packet >> message;

    std::cout << message.reason << '\n';
    socketComponent.m_tcpSocket.disconnect();
}

void NetworkSystem::ClientHandleDeathEvent(ClientSocketComponent& socketCompoment, DeathEventMessage& message)
{
    // @TODO: Update this to add an on screen message for the player that was killed and who killed him
    NetworkHelpers::KillEntity(message.m_playerId);
}

// This will mainly be used to determine if we still have stable connection and ping checks
void NetworkSystem::SendUpdate()
{
    Level* level = Application::Instance->GetCurrentLevel().get();

    if (level->IsServer())
    {
        ServerSocketComponent& socketComponent = level->GetComponent<ServerSocketComponent>(NETWORK_ENTITY);

        for (int i = 0; i < socketComponent.m_tcpSockets.size(); ++i)
        {
			sf::Packet packet;
			packet << PING_EVENTID;
            packet << i;

            socketComponent.m_tcpSockets[i]->send(packet);
            socketComponent.m_tcpSockets[i].m_waitingForResponse = true;
            socketComponent.m_tcpSockets[i].m_lastUpdateTime = level->GetGameTime();
        }
    }
}