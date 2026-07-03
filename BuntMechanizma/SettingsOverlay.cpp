// SettingsOverlay.cpp
#include "SettingsOverlay.h"
#include "Game.h"
#include <algorithm>

SettingsOverlay::SettingsOverlay() {
    font.openFromFile("Assets/GlasstownNbpRegular-RyMM.ttf");

    dimRect.setFillColor(sf::Color(0, 0, 0, 150));

    backgroundRect.setSize({ 600.f, 500.f });
    backgroundRect.setFillColor(sf::Color(40, 44, 52, 255));
    backgroundRect.setOutlineThickness(4.f);
    backgroundRect.setOutlineColor(sf::Color(115, 150, 82, 255));

    titleText.emplace(font, rus("НАСТРОЙКИ"), 40);
    sfxText.emplace(font, rus("Громкость SFX:"), 30);
    bgmText.emplace(font, rus("Громкость BGM:"), 30);
    skipText.emplace(font, rus("Пропустить сюжет:"), 30);

    sfxDown.setSize({ 40.f, 40.f }); sfxDown.setFillColor(sf::Color::Red);
    sfxUp.setSize({ 40.f, 40.f });   sfxUp.setFillColor(sf::Color::Green);

    bgmDown.setSize({ 40.f, 40.f }); bgmDown.setFillColor(sf::Color::Red);
    bgmUp.setSize({ 40.f, 40.f });   bgmUp.setFillColor(sf::Color::Green);

    checkbox.setSize({ 40.f, 40.f });

    closeBtn.setSize({ 200.f, 50.f });
    closeBtn.setFillColor(sf::Color(60, 64, 72));

    closeText.emplace(font, rus("ЗАКРЫТЬ"), 28);
}

void SettingsOverlay::update(sf::RenderWindow& window, GameSettings& settings, bool& closeOverlay, std::optional<sf::Sound>& clickSound) {
    sf::Vector2f viewSize = window.getView().getSize();
    float bgWidth = 600.f;
    float bgHeight = 500.f;
    float bgX = (viewSize.x - bgWidth) / 2.f;
    float bgY = (viewSize.y - bgHeight) / 2.f;

    sfxDown.setPosition({ bgX + 390.f, bgY + 130.f });
    sfxUp.setPosition({ bgX + 520.f, bgY + 130.f });
    bgmDown.setPosition({ bgX + 390.f, bgY + 210.f });
    bgmUp.setPosition({ bgX + 520.f, bgY + 210.f });
    checkbox.setPosition({ bgX + 455.f, bgY + 290.f });
    closeBtn.setPosition({ bgX + 200.f, bgY + 390.f });

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (!pressed) {
        mouseWasPressed = false;
        return;
    }
    if (mouseWasPressed) return;
    mouseWasPressed = true;

    if (closeBtn.getGlobalBounds().contains(mousePos)) {
        clickSound->play();
        closeOverlay = true;
    }
    if (sfxDown.getGlobalBounds().contains(mousePos)) {
        settings.sfxVolume = std::max(0.f, settings.sfxVolume - 10.f);
        clickSound->setVolume(settings.sfxVolume);
        clickSound->play();
    }
    if (sfxUp.getGlobalBounds().contains(mousePos)) {
        settings.sfxVolume = std::min(100.f, settings.sfxVolume + 10.f);
        clickSound->setVolume(settings.sfxVolume);
        clickSound->play();
    }
    if (bgmDown.getGlobalBounds().contains(mousePos)) {
        settings.bgmVolume = std::max(0.f, settings.bgmVolume - 10.f);
        clickSound->play();
    }
    if (bgmUp.getGlobalBounds().contains(mousePos)) {
        settings.bgmVolume = std::min(100.f, settings.bgmVolume + 10.f);
        clickSound->play();
    }
    if (checkbox.getGlobalBounds().contains(mousePos)) {
        clickSound->play();
        settings.skipStory = !settings.skipStory;
    }
}

void SettingsOverlay::render(sf::RenderWindow& window, const GameSettings& settings) {
    sf::Vector2f viewSize = window.getView().getSize();
    float bgWidth = 600.f;
    float bgHeight = 500.f;
    float bgX = (viewSize.x - bgWidth) / 2.f;
    float bgY = (viewSize.y - bgHeight) / 2.f;

    dimRect.setSize(viewSize);
    backgroundRect.setPosition({ bgX, bgY });

    if (titleText) {
        sf::FloatRect textBounds = titleText->getLocalBounds();
        titleText->setPosition({ bgX + (bgWidth - textBounds.size.x) / 2.f, bgY + 30.f });
    }

    if (sfxText)  sfxText->setPosition({ bgX + 40.f, bgY + 130.f });
    if (bgmText)  bgmText->setPosition({ bgX + 40.f, bgY + 210.f });
    if (skipText) skipText->setPosition({ bgX + 40.f, bgY + 290.f });

    if (closeText) {
        sf::FloatRect textBounds = closeText->getLocalBounds();
        closeText->setPosition({ bgX + 200.f + (200.f - textBounds.size.x) / 2.f, bgY + 390.f + (50.f - textBounds.size.y) / 2.f - 5.f });
    }

    window.draw(dimRect);
    window.draw(backgroundRect);

    if (titleText) window.draw(*titleText);
    if (sfxText) {
        sfxText->setString(rus("Громкость SFX: ") + std::to_string((int)settings.sfxVolume) + "%");
        window.draw(*sfxText);
    }
    if (bgmText) {
        bgmText->setString(rus("Громкость BGM: ") + std::to_string((int)settings.bgmVolume) + "%");
        window.draw(*bgmText);
    }
    if (skipText) window.draw(*skipText);

    window.draw(sfxDown); window.draw(sfxUp);
    window.draw(bgmDown); window.draw(bgmUp);

    checkbox.setFillColor(settings.skipStory ? sf::Color::Green : sf::Color::Transparent);
    checkbox.setOutlineThickness(2.f);
    checkbox.setOutlineColor(sf::Color::White);
    window.draw(checkbox);

    window.draw(closeBtn);
    if (closeText) window.draw(*closeText);
}