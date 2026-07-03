#pragma once
#ifndef SETTINGSOVERLAY_H
#define SETTINGSOVERLAY_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include "GameSettings.h"

class SettingsOverlay {
private:
    sf::RectangleShape backgroundRect;
    sf::RectangleShape dimRect;
    sf::Font font;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> sfxText;
    std::optional<sf::Text> bgmText;
    std::optional<sf::Text> skipText;

    sf::RectangleShape sfxDown, sfxUp;
    sf::RectangleShape bgmDown, bgmUp;
    sf::RectangleShape checkbox;
    sf::RectangleShape closeBtn;
    std::optional<sf::Text> closeText;

    bool mouseWasPressed = false;

public:
    SettingsOverlay();
    void update(sf::RenderWindow& window, GameSettings& settings, bool& closeOverlay, std::optional<sf::Sound>& clickSound);
    void render(sf::RenderWindow& window, const GameSettings& settings);
};

#endif