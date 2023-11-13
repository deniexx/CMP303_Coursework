#pragma once

struct MessagePlayerJoined
{
    int playerID;
};

// @TODO: Queue of packets and send them in case they need to arrive in order
// Map player IDs to queued packets, that way we can make sure each player receives their packages in order (only if we need to do so)
// We can use TCP for player input updates, if necessary, the server can try to update players with their position over UDP, hit visuals should also be over UDp
// Hit reg should use TCP