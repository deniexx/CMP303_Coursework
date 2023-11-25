#pragma once

#include <string>

#include "ISystem.h"

struct PhysicsUpdateMessage;
struct NewPlayerMessage;

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
    float TimeBetweenUpdates() override { return 99999999.f; }
    void SendUpdate() override {}

private:

    void ServerUpdate();
    void ClientUpdate();
    
    ServerSocketComponent& GetServerSocketComponent();
    ClientSocketComponent& GetClientSocketComponent();

    void RedistributePacket(ServerSocketComponent& socketComponent, sf::TcpSocket* client, sf::Packet& packet);
    
    /* ------------------------ SERVER ------------------------ */
    void ServerAcceptConnections(ServerSocketComponent& socketComponent);
    void ServerReceivePackets(ServerSocketComponent& socketComponent);
    void ServerCheckAuthentication(ServerSocketComponent& socketComponent, sf::TcpSocket* socket, const AuthenticationMessage& message);
    void ServerUpdateInputArrays(ServerSocketComponent& socketComponent, sf::TcpSocket* client, sf::Packet& packet);

    /* ------------------------ CLIENT ------------------------ */
    void ClientReceivePackets(ClientSocketComponent& socketComponent);
    void ClientCheckAuthentication(ClientSocketComponent& socketComponent, const std::string& authenticationMessage);
    void ClientNewPlayerEvent(ClientSocketComponent& socketComponent, const NewPlayerMessage& message);
    void ClientProcessInputReceived(ClientSocketComponent& socketComponent, sf::Packet& packet);
};
