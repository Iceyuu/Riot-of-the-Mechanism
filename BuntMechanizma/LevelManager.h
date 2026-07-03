#pragma once
#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "Level.h"
#include "GameSettings.h"
#include <memory>
#include <cstdint>
#include <SFML/Graphics.hpp>

class LevelManager
{
private:
    enum class FadeState { None, FadingOut, FadingIn };

    int currentLevel;
    std::unique_ptr<Level> activeLevel;
    sf::Font font;

    FadeState fadeState = FadeState::None;
    float fadeAlpha = 0.f;
    sf::RectangleShape fadeRect;
    sf::Clock fadeClock;
    bool isFadeRectInitialized = false;

    const GameSettings& settings;

public:
    LevelManager(const GameSettings& gameSettings);

    void loadProgress();
    void saveProgress();

    void nextLevel();
    void resetToFirstLevel();
    int getCurrentLevel() const;

    void startLevel();
    void unloadLevel();

    void pauseMusic();
    void resumeMusic();
    void stopMusic();

    void update(sf::RenderWindow& window, bool& backToMenu);
    void render(sf::RenderWindow& window);
};

#endif