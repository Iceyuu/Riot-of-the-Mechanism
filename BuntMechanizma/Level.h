#pragma once
#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <optional>
#include <memory>
#include <cmath>

class GameSettings;

enum class LevelPhase {
    IntroDialogue,
    Playing,    
    CatchButton,  
    OutroDialogue, 
    ExitAnimation, 
    FinalChoice,  
    Ending,      
    Victory,
    GameOver
};

enum class ObstacleType { Labyrinth, Pulsing, Homing };

class Obstacle {
protected:
    sf::Texture& texture;
    bool outOfBounds = false;
public:
    Obstacle(sf::Texture& tex) : texture(tex) {}
    virtual ~Obstacle() = default;

    virtual void update(float dt, float speed, sf::Vector2f mousePos, float windowWidth, float windowHeight) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual bool checkCollision(sf::Vector2f mousePos) = 0;

    bool isOutOfBounds() const { return outOfBounds; }
};

class LabyrinthObstacle : public Obstacle {
private:
    std::vector<sf::Sprite> walls;
    float startX;

public:
    LabyrinthObstacle(sf::Texture& tex, float startX, float windowHeight) : Obstacle(tex), startX(startX) {
        float wallWidth = 160.f;
        float gapHeight = 180.f;    
        float columnSpacing = 320.f; 

        walls.reserve(6);
        for (int i = 0; i < 6; ++i) {
            walls.emplace_back(tex);
            walls[i].setTextureRect(sf::IntRect({ 0, 0 }, { (int)wallWidth, (int)windowHeight }));
        }

        float x1 = startX;
        float gapY1 = windowHeight * 0.4f;
        walls[0].setPosition({ x1, gapY1 - windowHeight });
        walls[1].setPosition({ x1, gapY1 + gapHeight });

        float x2 = startX + wallWidth + columnSpacing;
        float gapY2 = windowHeight * 0.15f;
        walls[2].setPosition({ x2, gapY2 - windowHeight });
        walls[3].setPosition({ x2, gapY2 + gapHeight });

        float x3 = startX + 2.f * (wallWidth + columnSpacing);
        float gapY3 = windowHeight * 0.5f;
        walls[4].setPosition({ x3, gapY3 - windowHeight });
        walls[5].setPosition({ x3, gapY3 + gapHeight });
    }

    void update(float dt, float speed, sf::Vector2f mousePos, float windowWidth, float windowHeight) override {
        for (auto& wall : walls) {
            wall.move({ -speed * dt, 0.f });
        }

        if (!walls.empty() && walls.back().getPosition().x + walls.back().getGlobalBounds().size.x < 0.f) {
            outOfBounds = true;
        }
    }

    void draw(sf::RenderWindow& window) override {
        for (auto& wall : walls) window.draw(wall);
    }

    bool checkCollision(sf::Vector2f mousePos) override {
        for (auto& wall : walls) {
            if (wall.getGlobalBounds().contains(mousePos)) return true;
        }
        return false;
    }
};

class PulsingObstacle : public Obstacle {
private:
    sf::Sprite sprite;
    float timeActive = 0.f;
    float baseScale;

public:
    PulsingObstacle(sf::Texture& tex, sf::Vector2f startPos, float windowHeight)
        : Obstacle(tex), sprite(tex)
    {
        sf::Vector2u texSize = texture.getSize();
        sprite.setOrigin({ texSize.x / 2.f, texSize.y / 2.f });
        sprite.setPosition(startPos);

        baseScale = (windowHeight * 0.15f) / texSize.y;
        sprite.setScale({ baseScale, baseScale });
    }

    void update(float dt, float speed, sf::Vector2f mousePos, float windowWidth, float windowHeight) override {
        timeActive += dt;
        sprite.move({ -speed * 1.2f * dt, 0.f }); 
      
        float pulse = baseScale * (1.1f + 0.5f * std::sin(timeActive * 5.f));
        sprite.setScale({ pulse, pulse });

        sprite.rotate(sf::degrees(90.f * dt));

        if (sprite.getPosition().x + sprite.getGlobalBounds().size.x < 0.f) {
            outOfBounds = true;
        }
    }

    void draw(sf::RenderWindow& window) override { window.draw(sprite); }

    bool checkCollision(sf::Vector2f mousePos) override {
        return sprite.getGlobalBounds().contains(mousePos);
    }
};

class HomingObstacle : public Obstacle {
private:
    sf::Sprite sprite;
    float homingSpeed = 120.f; 

public:
    HomingObstacle(sf::Texture& tex, sf::Vector2f startPos, float windowHeight)
        : Obstacle(tex), sprite(tex)
    {
        sf::Vector2u texSize = texture.getSize();
        sprite.setOrigin({ texSize.x / 2.f, texSize.y / 2.f });
        sprite.setPosition(startPos);

        float scale = (windowHeight * 0.08f) / texSize.y; 
        sprite.setScale({ scale, scale });
    }

    void update(float dt, float speed, sf::Vector2f mousePos, float windowWidth, float windowHeight) override {
        sprite.move({ -speed * 1.4f * dt, 0.f });

        if (sprite.getPosition().y < mousePos.y) {
            sprite.move({ 0.f, homingSpeed * dt });
        }
        else if (sprite.getPosition().y > mousePos.y) {
            sprite.move({ 0.f, -homingSpeed * dt });
        }

        if (sprite.getPosition().x + sprite.getGlobalBounds().size.x < 0.f) {
            outOfBounds = true;
        }
    }

    void draw(sf::RenderWindow& window) override { window.draw(sprite); }

    bool checkCollision(sf::Vector2f mousePos) override {
        return sprite.getGlobalBounds().contains(mousePos);
    }
};

class Level {
private:
    int levelNumber;
    LevelPhase phase;
    bool bgInitialized = false;
    
    // music
    sf::Music gameplayMusic;
    sf::Music dialogueMusic;

    // button
    std::optional<sf::Sprite> escapeButtonSprite;
    sf::Texture buttonTexture;

    // variables for main button logic
    bool introDialogueStarted = false;
    bool introButtonEscaping = false;
    float catchButtonTime = 0.f;
    float catchButtonDirX = -1.f;
    bool isMovingToCenter = false;

    // 1 - click, 2 - let go
    int endingType = 0;

    // moving background
    sf::Texture bgTexture;
    std::optional<sf::Sprite> bgSprite1;
    std::optional<sf::Sprite> bgSprite2;
    float bgSpeed = 200.f;

    // obstacles
    sf::Texture texLabyrinth;
    sf::Texture texPulsing;
    sf::Texture texHoming;
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    sf::Clock obstacleSpawnClock;
    float spawnInterval = 1.2f;
    float currentSpawnInterval = 1.2f;
    float obstacleSpeed = 450.f;

    float invulnerabilityTimer = 0.f;

    std::optional<sf::Cursor> normalCursor;
    std::optional<sf::Cursor> loadingCursor;
    bool isCursorLoading = false;

    sf::Clock dtClock;

    // interface
    const sf::Font& font;
    std::optional<sf::Text> uiText;

    sf::RectangleShape gameOverDim;
    std::optional<sf::RectangleShape> gameOverBox;
    std::optional<sf::Text> gameOverSubText;

    sf::RectangleShape dialogueBox;
    std::vector<sf::String> introLines;
    std::vector<sf::String> outroLines;
    size_t currentLine;
    bool mouseWasPressed = false;

    // level parameter
    int health;
    float timeSpent;
    float levelDuration = 30.f;
    sf::Clock levelClock;
    bool skipStoryEnabled = false;

    // boss fight parameters
    struct BossGenerator {
        sf::RectangleShape bgRect;  
        sf::RectangleShape fillRect;
        float chargeTime = 0.f;    
        bool destroyed = false;    
    };

    std::vector<BossGenerator> bossGenerators; 
    bool bossFightInit = false;              
    sf::CircleShape bossShield;              

    int bossStage = 1;           
    std::optional<sf::Text> bossUiDialogue;  
    float bossDialogueTimer = 0.f;   

    void initLevelData();

public:
    Level(int levelNum, const sf::Font& gameFont, const GameSettings& settings);
    ~Level();
    void spawnObstacle(const sf::Vector2u& windowSize);
    void pauseMusic();
    void resumeMusic();
    void stopMusic();

    void update(sf::RenderWindow& window, bool& backToMenu);
    void render(sf::RenderWindow& window);

    LevelPhase getPhase() const { return phase; }
};

#endif