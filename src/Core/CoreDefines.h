#pragma once

#include <cstdint>

typedef uint32_t Entity;
 
#define NETWORK_ENTITY 0
#define ACCEPTABLE_POSITION_DELTA 100.f
#define AUTHENTICATION_MESSAGE_SERVER "1224MaikalRoko"
#define AUTHENTICATION_MESSAGE_CLIENT "ShtukuRukuMaruku"

enum class PlayerConnectionType : int8_t
{
    Server = 0,
    ClientLocal = 1,
    ClientRemote = 2,
    None = 3
};

enum class GameLevel : int8_t
{
    MainMenu = 0,
    Gameplay = 1
};