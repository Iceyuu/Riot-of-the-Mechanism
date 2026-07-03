#include "LevelManager.h"
#include <fstream>
#include <iostream>

LevelManager::LevelManager(const GameSettings& gameSettings)
    : settings(gameSettings)
{
    currentLevel = 1;
    font.openFromFile("Assets/GlasstownNbpRegular-RyMM.ttf");
    loadProgress();
}

void LevelManager::loadProgress()
{
    std::ifstream file("save.dat");
    if (!file.is_open()) { currentLevel = 1; return; }
    file >> currentLevel;
    if (currentLevel < 1 || currentLevel > 5) currentLevel = 1;
    file.close();
}

void LevelManager::saveProgress()
{
    std::ofstream file("save.dat");
    if (!file.is_open()) return;
    file << currentLevel;
    file.close();
}

void LevelManager::resetToFirstLevel() {
    currentLevel = 1;
    saveProgress();
}

void LevelManager::startLevel() {
    activeLevel = std::make_unique<Level>(currentLevel, font, settings);
}

void LevelManager::pauseMusic() {
    if (activeLevel) activeLevel->pauseMusic();
}

void LevelManager::resumeMusic() {
    if (activeLevel) activeLevel->resumeMusic();
}

void LevelManager::stopMusic() {
    if (activeLevel) activeLevel->stopMusic();
}

void LevelManager::nextLevel()
{
    currentLevel++;
    if (currentLevel > 5) {
        currentLevel = 1;
    }
    saveProgress();
    startLevel();
}

void LevelManager::unloadLevel() {
    activeLevel.reset();
}

int LevelManager::getCurrentLevel() const { return currentLevel; }

void LevelManager::update(sf::RenderWindow& window, bool& backToMenu) {
    if (!isFadeRectInitialized) {
        fadeRect.setSize(sf::Vector2f(window.getSize()));
        fadeRect.setFillColor(sf::Color(0, 0, 0, 0));
        isFadeRectInitialized = true;
    }

    float dt = fadeClock.restart().asSeconds();
    if (dt > 0.1f) dt = 0.1f;


    if (fadeState == FadeState::FadingOut) {
        fadeAlpha += 450.f * dt; 
        if (fadeAlpha >= 255.f) {
            fadeAlpha = 255.f;
            nextLevel(); 
            fadeState = FadeState::FadingIn; 
        }
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
        return; 
    }

 
    if (fadeState == FadeState::FadingIn) {
        fadeAlpha -= 450.f * dt;
        if (fadeAlpha <= 0.f) {
            fadeAlpha = 0.f;
            fadeState = FadeState::None;
        }
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
    }

    if (activeLevel) {
        activeLevel->update(window, backToMenu);

        if (backToMenu) {
            unloadLevel();
            fadeState = FadeState::None;
            return;
        }
        if (activeLevel->getPhase() == LevelPhase::Victory && fadeState == FadeState::None) {
            fadeState = FadeState::FadingOut;
            fadeAlpha = 0.f;
        }
    }
}

void LevelManager::render(sf::RenderWindow& window) {
    if (activeLevel) {
        activeLevel->render(window);
    }
    if (fadeState != FadeState::None) {
        window.draw(fadeRect);
    }
}