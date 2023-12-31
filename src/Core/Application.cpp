#include "Application.h"

#include <fstream>
#include "MainMenuLevel.h"
#include "TextureID.h"
#include "../Core/GameplayLevel.h"
#include <fstream>

void Application::StartUp()
{
	Instance = this;
	FillTextureRegister();
	m_font.loadFromFile("assets/6809chargen.ttf");
	std::ifstream f("assets/playerName.txt");
	getline(f, Application::Instance->m_playerName);
	f.close();

	m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1280, 720), "Alehandro Mix");
	sf::Clock deltaClock;

	m_window->setFramerateLimit(60);
	m_window->setVerticalSyncEnabled(false);

	std::ifstream file("assets/playerName.txt");
	getline(file, m_playerName);

	m_currentLevel = std::make_shared<MainMenuLevel>();
	m_currentLevel->Begin();

	while (m_window->isOpen())
	{
		float deltaTime = deltaClock.restart().asSeconds();
		m_currentLevel->Update(deltaTime);
		PollEvents(m_window);
		m_window->clear(sf::Color(100, 149, 237));
		m_window->setActive();
		m_currentLevel->Render();
		m_window->display();

		if (m_currentGameLevel != m_desiredGameLevel)
		{
			m_currentGameLevel = m_desiredGameLevel;
			if (m_currentGameLevel == GameLevel::MainMenu)
			{
				m_currentLevel = std::make_shared<MainMenuLevel>();
				m_currentLevel->Begin();
				
			}
			else
			{
				m_currentLevel = std::make_shared<GameplayLevel>();
				m_currentLevel->Begin();
			}
		}
	}
}

std::shared_ptr<Level> Application::GetCurrentLevel()
{
	return m_currentLevel;
}

bool Application::IsFocused() const
{
	return m_window->hasFocus();
}

void Application::SwitchLevel(GameLevel newLevel)
{
	m_desiredGameLevel = newLevel;
}

sf::Vector2u Application::GetWindowSize()
{
	return m_window->getSize();
}

void Application::PollEvents(std::shared_ptr<sf::RenderWindow> window)
{
	sf::Event currentEvent;
	while (window->pollEvent(currentEvent))
	{
		switch (currentEvent.type)
		{
			case sf::Event::Closed:
				window->close();
				break;
		case sf::Event::KeyPressed:
				if (currentEvent.key.scancode == sf::Keyboard::Scancode::Escape)
				{
					m_currentLevel->DisconnectLocalPlayer();
					exit(0);
				}
				break;
			case sf::Event::KeyReleased:
				// @TODO: Add a key released event response (input component)
				break;
			case sf::Event::MouseButtonPressed:
				break;
			case sf::Event::MouseButtonReleased:
				break;
			case sf::Event::MouseMoved:
				break;
		}
	}
}

void Application::FillTextureRegister()
{
	m_textureRegister[rahmiTexID].loadFromFile("assets/Rahmi.png");
}
