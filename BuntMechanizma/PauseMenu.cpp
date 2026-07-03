#include "PauseMenu.h"
#include "Game.h"

PauseMenu::PauseMenu()
{
    font.openFromFile("Assets/GlasstownNbpRegular-RyMM.ttf");
    buttonTexture.loadFromFile("Assets/menubutton_default.png");

    dimRect.setFillColor(sf::Color(0, 0, 0, 165));

    backgroundRect.setSize({ 420.f, 320.f });
    backgroundRect.setFillColor(sf::Color(40, 44, 52, 255));
    backgroundRect.setOutlineThickness(4.f);
    backgroundRect.setOutlineColor(sf::Color(115, 150, 82, 255));

    titleText.emplace(font, rus("ПАУЗА"), 42);
    titleText->setFillColor(sf::Color::White);
    
}

void PauseMenu::updateLayout(sf::Vector2f viewSize)
{
    if (viewSize == lastSize && !buttons.empty())
        return;

    lastSize = viewSize;
    buttons.clear();

    float panelWidth = backgroundRect.getSize().x;
    float panelHeight = backgroundRect.getSize().y;

    // look for the center of the screen
    float panelX = (viewSize.x - panelWidth) / 2.f;
    float panelY = (viewSize.y - panelHeight) / 2.f;
    backgroundRect.setPosition({ panelX, panelY });

    // centre the title text
    if (titleText) {
        sf::FloatRect textBounds = titleText->getLocalBounds();
        titleText->setPosition({ panelX + (panelWidth - textBounds.size.x) / 2.f, panelY + 25.f });
    }

    // centre the buttons
    float btnWidth = 235.f;
    float btnX = (viewSize.x - btnWidth) / 2.f;

    buttons.emplace_back(ButtonType::Play, buttonTexture, font, "Продолжить", btnX, panelY + 110.f);
    buttons.emplace_back(ButtonType::Exit, buttonTexture, font, "Главное меню", btnX, panelY + 195.f);
}

std::optional<ButtonType> PauseMenu::update(sf::RenderWindow& window)
{
    sf::Vector2f viewSize = window.getView().getSize();
    updateLayout(viewSize);

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // hover effect for buttons
    for (auto& button : buttons)
    {
        button.updateHover(mousePos);
    }

    // check for click
    bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (!pressed)
    {
        mouseWasPressed = false;
        return std::nullopt;
    }

    if (mouseWasPressed)
        return std::nullopt;

    mouseWasPressed = true;

    for (const auto& button : buttons)
    {
        if (button.contains(mousePos))
            return button.getType();
    }

    return std::nullopt;
}

void PauseMenu::render(sf::RenderWindow& window)
{
    sf::Vector2f viewSize = window.getView().getSize();
    updateLayout(viewSize); 

    // darken the backgorund
    dimRect.setSize(viewSize);
    window.draw(dimRect);

    // render the menu
    window.draw(backgroundRect);

    // render the text
    if (titleText) {
        window.draw(*titleText);
    }

    // render the buttons
    for (auto& button : buttons)
        button.draw(window);
}