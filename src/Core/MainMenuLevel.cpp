#include "MainMenuLevel.h"
#include "Application.h"
#include "TextureID.h"

#include "../Systems/MainMenuSystem.h"

void MainMenuLevel::Begin()
{
    Level::Begin();

    //Entity background = CreateEntity();
    Entity hostButton = CreateEntity();
    Entity joinButton = CreateEntity();
    Entity quitButton = CreateEntity();

    errorMessage = CreateEntity();

    //sf::Texture backgroundTex;

    //EmplaceComponent<SpriteComponent>(background).m_sprite.setTexture(Application::Instance->m_textureRegister[backgroundTexID]);
    TextComponent& textComp1 = EmplaceComponent<TextComponent>(hostButton);
    TextComponent& textComp2 = EmplaceComponent<TextComponent>(joinButton);
    TextComponent& textComp3 = EmplaceComponent<TextComponent>(quitButton);
    TextComponent& textComp4 = EmplaceComponent<TextComponent>(errorMessage);

    textComp1.text.setString("H - Host");
    textComp2.text.setString("J - Join");
    textComp3.text.setString("ESC - Quit");

    textComp1.text.setFont(Application::Instance->m_font);
    textComp2.text.setFont(Application::Instance->m_font);
    textComp3.text.setFont(Application::Instance->m_font);
    textComp4.text.setFont(Application::Instance->m_font);

    textComp1.text.setColor(sf::Color(255, 255, 255));
    textComp2.text.setColor(sf::Color(255, 255, 255));
    textComp3.text.setColor(sf::Color(255, 255, 255));
    textComp4.text.setColor(sf::Color(255, 0, 0));

    TransformComponent& transformComp1 = GetComponent<TransformComponent>(hostButton);
    TransformComponent& transformComp2 = GetComponent<TransformComponent>(joinButton);
    TransformComponent& transformComp3 = GetComponent<TransformComponent>(quitButton);
    TransformComponent& transformComp4 = GetComponent<TransformComponent>(errorMessage);

    transformComp1.m_x = 150.f;
    transformComp1.m_y = 360.f;
    transformComp2.m_x = 150.f;
    transformComp2.m_y = 420.f;
    transformComp3.m_x = 150.f;
    transformComp3.m_y = 480.f;
    transformComp4.m_x = 150.f;
    transformComp4.m_y = 120.f;

    AddSystem<MainMenuSystem>();
}

void MainMenuLevel::Render()
{
    TextComponent& errorM = GetComponent<TextComponent>(errorMessage);
    errorM.text.setString(Application::Instance->m_errorMessage);

    Level::Render();
}
