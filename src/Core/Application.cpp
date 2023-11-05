#include "Application.h"

void Application::StartUp()
{
	Instance = this;

	window = new sf::RenderWindow(sf::VideoMode(1280, 720), "Alehandro Mix");
	sf::Clock deltaClock;

	window->setFramerateLimit(60);
	window->setVerticalSyncEnabled(false);

	currentLevel = std::make_shared<Level>();
	currentLevel->Begin();

	while (window->isOpen())
	{
		float deltaTime = deltaClock.restart().asSeconds();
		window->clear();
		PollEvents(window);
		currentLevel->Update(deltaTime);

		window->setActive();
		currentLevel->Render();
		window->display();
	}
}

void Application::PollEvents(sf::RenderWindow* window)
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
