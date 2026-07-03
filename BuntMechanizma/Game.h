#pragma once

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <cstdint>
#include <string>

#include "Menu.h"
#include "PauseMenu.h"
#include "LevelManager.h"
#include "Level.h"
#include "GameSettings.h"
#include "SettingsOverlay.h"

	//Class that acts as the game engine
	//Wrapper class.

class Game
{
private:
    enum class State { Menu, Playing, Pause, Settings };

    State state;
    Menu menu;

    sf::Music menuMusic;
    PauseMenu pauseMenu;
    sf::SoundBuffer clickBuffer;
    std::optional<sf::Sound> clickSound;

    GameSettings settings;
    SettingsOverlay settingsOverlay;
    LevelManager levelManager;

    sf::RenderWindow* window;
    sf::VideoMode videoMode;

    // smooth transition between levels
    sf::Clock fadeClock;
    sf::RectangleShape fadeRect;
    bool isFadingOut = false;
    bool isFadingIn = false;
    float fadeAlpha = 0.f;
    State nextState = State::Menu;

    void initVariables();
    void initWindow();
public:
    Game();
    virtual ~Game();
    const bool running() const;
    void pollEvents();
    void update();
    void render();
};

inline sf::String rus(const std::string& utf8Str) {
    return sf::String::fromUtf8(utf8Str.begin(), utf8Str.end());
}

#endif