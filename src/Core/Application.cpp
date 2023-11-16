#include "Application.h"

void Application::StartUp()
{
	Instance = this;

	m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1280, 720), "Alehandro Mix");
	sf::Clock deltaClock;

	m_window->setFramerateLimit(60);
	m_window->setVerticalSyncEnabled(false);

	m_currentLevel = std::make_shared<Level>();
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
	}
}

std::shared_ptr<Level> Application::GetCurrentLevel()
{
	return m_currentLevel;
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
				// @TODO: Add a key pressed event response (input component)
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

void Application::UpdateNetwork()
{
	// @TODO: Run a network sim on the components
}

void Application::SimulateGame()
{
	// @TODO: Simulate the game
}

void Application::Render()
{
	// @TODO: Render all sprite components, etc...
}
