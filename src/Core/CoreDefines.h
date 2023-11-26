#pragma once

#include <cstdint>

typedef uint32_t Entity;

#define NETWORK_ENTITY 0
#define ACCEPTABLE_POSITION_DELTA 10.f
#define AUTHENTICATION_MESSAGE_SERVER "Alehandro"
#define AUTHENTICATION_MESSAGE_CLIENT "Ruzmarin"

enum class PlayerConnectionType : int8_t
{
    Server = 0,
    ClientLocal = 1,
    ClientRemote = 2,
    None = 3
};
