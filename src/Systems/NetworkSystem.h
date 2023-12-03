#pragma once

#include <string>

#include "ISystem.h"

struct DeathEventMessage;
struct PhysicsUpdateMessage;
struct NewPlayerMessage;
struct TCPSocket;

#define MAX_PING_ALLOWED 0.512f

namespace sf
{
    class Packet;
    class TcpSocket;
}

struct AuthenticationMessage;
struct ClientSocketComponent;
struct ServerSocketComponent;

class NetworkSystem : public ISystem
{
public:

    void BeginSystem() override;
    void UpdateSystem(float deltaTime) override;
    void DestroySystem() override;

    // We don't really want to send updates from this, so we set this number to a very high one
    // Updates from this system only happen on game events at the current moment
    // So we don't need to send updates periodically
    float TimeBetweenUpdates() override { return 3.f; }
    void SendUpdate() override;

private:

    void ServerUpdate(float deltaTime);
    void ClientUpdate();
    
    ServerSocketComponent& GetServerSocketComponent();
    ClientSocketComponent& GetClientSocketComponent();

	void RedistributePacket(ServerSocketComponent& socketComponent, TCPSocket& client, sf::Packet& packet);
    
    /* ------------------------ SERVER ------------------------ */
    void ServerCheckConnections(ServerSocketComponent& socketComponent, float deltaTime);
    void ServerAcceptConnections(ServerSocketComponent& socketComponent);
    void ServerReceivePackets(ServerSocketComponent& socketComponent);
    void ServerCheckAuthentication(ServerSocketComponent& socketComponent, TCPSocket& socket, const AuthenticationMessage& message);
    void ServerUpdateInputArrays(ServerSocketComponent& socketComponent, TCPSocket& client, sf::Packet& packet);
    void ServerUpdateHitReg(ServerSocketComponent& socketComponent, TCPSocket& client, sf::Packet& packet);
    void ServerHandlePingEvent(ServerSocketComponent& socketComponent, TCPSocket& client, sf::Packet& packet);

    /* ------------------------ CLIENT ------------------------ */
    void ClientReceivePackets(ClientSocketComponent& socketComponent);
    void ClientCheckAuthentication(ClientSocketComponent& socketComponent, const std::string& authenticationMessage);
    void ClientNewPlayerEvent(ClientSocketComponent& socketComponent, const NewPlayerMessage& message);
    void ClientProcessInputReceived(ClientSocketComponent& socketComponent, sf::Packet& packet);
    void ClientProcessFailedAuthentication(ClientSocketComponent& socketComponent, sf::Packet& packet);
    void ClientHandleDeathEvent(ClientSocketComponent& socketCompoment, DeathEventMessage& message);
};
