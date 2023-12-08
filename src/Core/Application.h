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
<<<<<<< HEAD

=======
>>>>>>> 0b187b06c71657d6b5d6142e1c5bba031ea0a4a4
	std::string m_playerName = "";
	
	/* Update Loops*/
private:

	GameLevel m_currentGameLevel = GameLevel::MainMenu;
	GameLevel m_desiredGameLevel = GameLevel::MainMenu;

	std::shared_ptr<sf::RenderWindow> m_window;

	std::shared_ptr<Level> m_currentLevel;

	void PollEvents(std::shared_ptr<sf::RenderWindow> window);

	void FillTextureRegister();
};

