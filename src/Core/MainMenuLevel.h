#pragma once

#include "Level.h"

class MainMenuLevel : public Level
{
    void Begin() override;
    void Render() override;

private:

    Entity errorMessage;
};