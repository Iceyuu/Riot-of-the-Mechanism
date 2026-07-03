#pragma once

#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <vector>
#include <optional>
#include <SFML/Graphics.hpp>

#include "Button.h"

class PauseMenu
{
private:
    bool mouseWasPressed = false;
    sf::Texture buttonTexture;
    sf::Texture buttonIconTexture;
    sf::Font font;
    std::vector<Button> buttons;
    sf::RectangleShape backgroundRect;
    std::optional<sf::Text> titleText;
    sf::RectangleShape dimRect;

    sf::Vector2f lastSize{ 0.f, 0.f };
    void updateLayout(sf::Vector2f viewSize);

public:
    PauseMenu();

    std::optional<ButtonType> update(sf::RenderWindow& window);

    void render(sf::RenderWindow& window);
};

#endif