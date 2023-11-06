#pragma once

#include <SFML/Graphics.hpp>
#include "Level.h"

class Application
{
public:

	friend class Level;
	inline static Application* Instance;

	/**
	 * Starts up the application
	 */
	void StartUp();

	/* Update Loops*/
private:

	std::shared_ptr<sf::RenderWindow> window;

	std::shared_ptr<Level> currentLevel;

	void PollEvents(std::shared_ptr<sf::RenderWindow> window);
	void UpdateNetwork();
	void SimulateGame();
	void Render();
};

