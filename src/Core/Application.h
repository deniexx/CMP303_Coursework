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

	void SwitchLevel(GameLevel newLevel);

	sf::Vector2u GetWindowSize();

	bool m_isServer;

	std::unordered_map<uint32_t, sf::Texture> m_textureRegister;
	sf::Font m_font;

	std::string m_errorMessage = "";
	
	/* Update Loops*/
private:

	GameLevel m_currentGameLevel = GameLevel::MainMenu;
	GameLevel m_desiredGameLevel = GameLevel::MainMenu;

	std::shared_ptr<sf::RenderWindow> m_window;

	std::shared_ptr<Level> m_currentLevel;

	void PollEvents(std::shared_ptr<sf::RenderWindow> window);

	void FillTextureRegister();
};

