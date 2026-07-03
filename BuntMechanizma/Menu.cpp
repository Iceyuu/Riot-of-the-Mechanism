#include "Menu.h"
#include "Game.h"
#include <filesystem>

Menu::Menu()
{
    font.openFromFile("Assets/GlasstownNbpRegular-RyMM.ttf");

    //preparing main button for rendering
    mainButtonTexture.loadFromFile("Assets/button.png");
    mainButtonSprite.emplace(mainButtonTexture);
    mainButtonSprite->setPosition({ 860.f, 300.f }); // Чуть приподняли логотип

    //preparing background image for rendering
    backgroundTexture.loadFromFile("Assets/menubackground.png");
    backgroundSprite.emplace(backgroundTexture);

    //preparing title text for rendering
    titleText.emplace(font, rus("БУНТ МЕХАНИЗМА"), 86); 
    titleText->setPosition({ 700.f, 150.f });
    titleText->setFillColor(sf::Color(115, 150, 82, 255));
    titleText->setStyle(sf::Text::Bold);
    titleText->setLetterSpacing(2.0f);

    buttonTexture.loadFromFile("Assets/menubutton_default.png");

    // prepare 5 buttons with a spacing of 75 pixels (y axis)
    buttons.emplace_back(ButtonType::Play, buttonTexture, font, "Начать игру", 842.5f, 430.f);
    buttons.emplace_back(ButtonType::Continue, buttonTexture, font, "Продолжить", 842.5f, 505.f);
    buttons.emplace_back(ButtonType::Settings, buttonTexture, font, "Настройки", 842.5f, 580.f);
    buttons.emplace_back(ButtonType::Help, buttonTexture, font, "Справка", 842.5f, 655.f);
    buttons.emplace_back(ButtonType::Exit, buttonTexture, font, "Выход", 842.5f, 730.f);

    // disabled overlay for if continue button is unavailable
    disabledOverlay.setSize(sf::Vector2f(static_cast<float>(buttonTexture.getSize().x), static_cast<float>(buttonTexture.getSize().y)));
    disabledOverlay.setPosition({ 842.5f, 505.f });
    disabledOverlay.setFillColor(sf::Color(30, 30, 30, 185));

    updateSaveStatus();
}

// check if user has save file
void Menu::updateSaveStatus()
{
    hasSave = std::filesystem::exists("save.dat");
}

std::optional<ButtonType> Menu::update(sf::RenderWindow& window)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // button appearance changes if cursor hovers over it
    for (auto& button : buttons)
    {
        // if there is no save, continue button does not react to hover
        if (button.getType() == ButtonType::Continue && !hasSave)
            continue;
        button.updateHover(mousePos);
    }

    bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (!pressed)
    {
        mouseWasPressed = false;
        return std::nullopt;
    }

    if (mouseWasPressed)
        return std::nullopt;

    mouseWasPressed = true;

    // button event if a button gets clicked on
    for (const auto& button : buttons)
    {
        // if button is continue & no save file, skip
        if (button.getType() == ButtonType::Continue && !hasSave)
            continue;

        if (button.contains(mousePos))
            // event is dictated by the type of button
            return button.getType();
    }

    return std::nullopt;
}

void Menu::render(sf::RenderWindow& window)
{
    if (backgroundSprite) {
        sf::FloatRect backgroundBounds = backgroundSprite->getLocalBounds();
        sf::Vector2f windowSize(window.getSize());
        backgroundSprite->setScale({
            windowSize.x / backgroundBounds.size.x,
            windowSize.y / backgroundBounds.size.y
            });
        window.draw(*backgroundSprite);
    }

    if (titleText)        window.draw(*titleText);
    if (mainButtonSprite) window.draw(*mainButtonSprite);

    for (auto& button : buttons) {
        button.draw(window);
    }

    if (!hasSave) {
        window.draw(disabledOverlay);
    }
}