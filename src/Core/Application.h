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

	sf::RenderWindow* window;

	std::shared_ptr<Level> currentLevel;

	void PollEvents(sf::RenderWindow* window);
	void UpdateNetwork();
	void SimulateGame();
	void Render();
};

