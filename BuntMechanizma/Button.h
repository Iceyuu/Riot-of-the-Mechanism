#pragma once

#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

enum class ButtonType
{
    Play,
    Continue,
    Settings,
    Help,
    Exit
};

class Button
{
private:
    ButtonType type;
    sf::Sprite sprite;
    sf::Sprite icon;
    sf::Text text;
    static std::map<ButtonType, sf::Texture> s_textureCache;
public:
    Button(
        ButtonType type,
        const sf::Texture& texture,
        const sf::Font& font,
        const std::string& caption,
        float x,
        float y
    );
    // Button.h
// ... (внутри секции public класса Button)
    static void initCache();
    static const sf::Texture& getIconTexture(ButtonType type); // <-- Добавляем эту строчку
    void setType(ButtonType type);
    void draw(sf::RenderWindow& window);
    void updateHover(sf::Vector2f mousePos);
    bool contains(sf::Vector2f mousePos) const;
    ButtonType getType() const;
};
#endif
