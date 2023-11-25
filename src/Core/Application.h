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

	std::shared_ptr<Level> GetCurrentLevel();

	bool IsFocused() const;
	
	/* Update Loops*/
private:

	std::shared_ptr<sf::RenderWindow> m_window;

	std::shared_ptr<Level> m_currentLevel;

	void PollEvents(std::shared_ptr<sf::RenderWindow> window);
};

