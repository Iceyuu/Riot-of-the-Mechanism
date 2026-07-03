#pragma once

#ifndef MENU_H
#define MENU_H

#include <vector>
#include <optional>
#include <string>
#include <SFML/Graphics.hpp>

#include "Button.h"

class Menu
{
private:
    bool mouseWasPressed = false;
    sf::Texture buttonTexture;
    sf::Texture buttonIconTexture;

    std::optional<sf::Sprite> mainButtonSprite;
    sf::Texture mainButtonTexture;

    std::vector<Button> buttons;

    sf::Font font;
    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;
    std::optional<sf::Text> titleText;

    bool hasSave = false;
    sf::RectangleShape disabledOverlay;

public:
    Menu();

    void updateSaveStatus();

    std::optional<ButtonType> update(sf::RenderWindow& window);

    void render(sf::RenderWindow& window);
};

#endif