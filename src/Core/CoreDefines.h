#pragma once

#include <cstdint>

typedef uint32_t Entity;

enum class PlayerConnectionType : uint8_t
{
    Server = 0,
    ClientLocal = 1,
    ClientRemote = 2,
    None = 3
};
