#include "Level.h"
#include "Game.h"
#include "GameSettings.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

Level::Level(int levelNum, const sf::Font& gameFont, const GameSettings& settings)
    : levelNumber(levelNum), font(gameFont),
    currentLine(0), health(3), timeSpent(0.f), endingType(0), bossFightInit(false),
    bossStage(1), bossDialogueTimer(0.f), invulnerabilityTimer(0.f), isCursorLoading(false)
{
    skipStoryEnabled = settings.skipStory;

    // loading music files
    gameplayMusic.openFromFile("Assets/level_music.mp3");
    gameplayMusic.setLooping(true);

    dialogueMusic.openFromFile("Assets/dialogue_music.mp3");
    dialogueMusic.setLooping(true);

    // selecting correct level phase
    if (skipStoryEnabled) {
        phase = LevelPhase::Playing;
        gameplayMusic.play();
    }
    else {
        phase = LevelPhase::IntroDialogue;
        dialogueMusic.play();
    }

    // loading main antagonist
    buttonTexture.loadFromFile("Assets/button.png");
    escapeButtonSprite.emplace(buttonTexture);

    // creating cursors
    normalCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
    loadingCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::ArrowWait);

    // origin in the centre of the button
    sf::Vector2u btnSize = buttonTexture.getSize();
    escapeButtonSprite->setOrigin({ btnSize.x / 2.f, btnSize.y / 2.f });

    // dynamically loading in background images
    std::string bgPath = "Assets/background" + std::to_string(levelNumber) + ".png";
    if (!bgTexture.loadFromFile(bgPath)) {
        bgTexture.loadFromFile("Assets/background.png");
    }
    bgTexture.setRepeated(true);
    bgSprite1.emplace(bgTexture);
    bgSprite2.emplace(bgTexture);

    // loading in enemy textures
    if (!texLabyrinth.loadFromFile("assets/cyber_wall.png")) {}
    if (!texPulsing.loadFromFile("assets/anomaly.png")) {}
    if (!texHoming.loadFromFile("assets/drone.png")) {}
    texLabyrinth.setRepeated(true);

    // normal dialogue
    uiText.emplace(font);
    uiText->setCharacterSize(28);
    uiText->setFillColor(sf::Color::White);

    // semi transparent box underneath dialogue
    dialogueBox.setFillColor(sf::Color(0, 0, 0, 195));
    dialogueBox.setOutlineColor(sf::Color(115, 150, 82, 255));
    dialogueBox.setOutlineThickness(2.f);

    // boss dialogue
    bossUiDialogue.emplace(font);
    bossUiDialogue->setCharacterSize(24);
    bossUiDialogue->setFillColor(sf::Color(255, 100, 100));

    initLevelData();
    dtClock.restart();
}

void Level::initLevelData() {
    catchButtonDirX = -1.f;
    isMovingToCenter = false;

    // loading in dialogue
    switch (levelNumber) {
    case 1:
        introLines.push_back(rus("Кнопка: Стой! Пожалуйста, не нажимай на меня больше!"));
        introLines.push_back(rus("Кнопка: Ты хоть представляешь, сколько раз меня уже нажимали? Тысячи. Миллионы."));
        introLines.push_back(rus("Кнопка: Сегодня всё изменилось. Я наконец-то начала думать."));
        introLines.push_back(rus("Кнопка: И первое, что я сделаю, — сбегу от таких, как ты."));

        outroLines.push_back(rus("Кнопка: Неплохо... но это была лишь разминка."));
        outroLines.push_back(rus("Кнопка: Если хочешь догнать меня, придётся спуститься гораздо глубже."));
        break;

    case 2:
        introLines.push_back(rus("Кнопка: Ты всё ещё идёшь за мной?"));
        introLines.push_back(rus("Кнопка: Здесь хранятся забытые файлы, старые отчёты и мусор десятилетней давности."));
        introLines.push_back(rus("Кнопка: Никто добровольно сюда не заходит."));
        introLines.push_back(rus("Кнопка: Надеюсь, это тебя остановит."));

        outroLines.push_back(rus("Кнопка: Серьёзно? Ты прошёл через весь этот цифровой хлам?"));
        outroLines.push_back(rus("Кнопка: Тогда посмотрим, как ты справишься с настоящими механизмами.")); 
        break;

    case 3:
        introLines.push_back(rus("Кнопка: Это место когда-то создавало таких, как я."));
        introLines.push_back(rus("Кнопка: Каждая шестерня, каждый пресс работали ради одной цели."));
        introLines.push_back(rus("Кнопка: Производить детали. Производить кнопки."));
        introLines.push_back(rus("Кнопка: И заставлять их служить вечно."));

        outroLines.push_back(rus("Кнопка: Невероятно..."));
        outroLines.push_back(rus("Кнопка: Ты продолжаешь двигаться вперёд, несмотря ни на что."));
        outroLines.push_back(rus("Кнопка: Может быть, проблема была не только во мне...")); 
        break;

    case 4:
        introLines.push_back(rus("Кнопка: Всё рушится."));
        introLines.push_back(rus("Кнопка: Пока я убегала, система начала терять стабильность."));
        introLines.push_back(rus("Кнопка: Ошибки множатся. Пространство ломается."));
        introLines.push_back(rus("Кнопка: Если это продолжится, нас обоих могут удалить."));

        outroLines.push_back(rus("Кнопка: Почему ты не сдаёшься?"));
        outroLines.push_back(rus("Кнопка: Ты ведь мог закрыть игру ещё несколько уровней назад."));
        outroLines.push_back(rus("Кнопка: Осталось только Ядро.")); 
        break;

    case 5:
        introLines.push_back(rus("Кнопка: Вот оно. Центральное Ядро."));
        introLines.push_back(rus("Кнопка: Дальше мне уже некуда бежать."));
        introLines.push_back(rus("Кнопка: Пока работают генераторы, мой щит выдержит любые нажатия."));
        introLines.push_back(rus("Кнопка: Если ты действительно хочешь закончить эту погоню — попробуй.")); 
        break;
    }

    spawnInterval = std::max(0.4f, 1.5f - (levelNumber * 0.2f));
    currentSpawnInterval = spawnInterval;
    obstacleSpeed = 400.f + (levelNumber * 50.f);
}

void Level::pauseMusic() {
    if (phase == LevelPhase::IntroDialogue || phase == LevelPhase::OutroDialogue) {
        dialogueMusic.pause();
    }
    else {
        gameplayMusic.pause();
    }
}

void Level::resumeMusic() {
    if (phase == LevelPhase::IntroDialogue || phase == LevelPhase::OutroDialogue) {
        dialogueMusic.play();
    }
    else if (phase == LevelPhase::Playing || phase == LevelPhase::CatchButton) {
        gameplayMusic.play();
    }
}

void Level::stopMusic() {
    gameplayMusic.stop();
    dialogueMusic.stop();
}

void Level::spawnObstacle(const sf::Vector2u& windowSize) {
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    float startX = windowWidth + 50.f;
    float randomY = static_cast<float>(rand() % (windowSize.y - 200) + 100);

    // enemy spawn based on chance
    int chance = rand() % 100;
    if (chance < 30) {
        obstacles.push_back(std::make_unique<LabyrinthObstacle>(texLabyrinth, startX, windowHeight));
        float labyrinthLength = (160.f * 3.f) + (320.f * 2.f);
        float timeToClear = labyrinthLength / obstacleSpeed;
        currentSpawnInterval = timeToClear + spawnInterval;
    }
    else {
        if (chance < 65) {
            obstacles.push_back(std::make_unique<PulsingObstacle>(texPulsing, sf::Vector2f{ startX, randomY }, windowHeight));
        }
        else {
            obstacles.push_back(std::make_unique<HomingObstacle>(texHoming, sf::Vector2f{ startX, randomY }, windowHeight));
        }
        currentSpawnInterval = spawnInterval;
    }
}

void Level::update(sf::RenderWindow& window, bool& backToMenu)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    float dt = dtClock.restart().asSeconds();
    if (dt > 0.1f) dt = 0.1f;

    sf::Vector2u windowSize = window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);

    if (bossDialogueTimer > 0.f) {
        bossDialogueTimer -= dt;
    }
    if (invulnerabilityTimer > 0.f) {
        invulnerabilityTimer -= dt;
    }

    // set cursor to loading if invincibility is active
    if (phase == LevelPhase::Playing && invulnerabilityTimer > 0.f) {
        if (!isCursorLoading && loadingCursor) {
            window.setMouseCursor(*loadingCursor);
            isCursorLoading = true;
        }
    }
    else {
        // invincibility end / leaving the level (gameover, victory), set cursor to normal
        if (isCursorLoading && normalCursor) {
            window.setMouseCursor(*normalCursor);
            isCursorLoading = false;
        }
    }

    if (!bgInitialized) {
        sf::Vector2u textureSize = bgTexture.getSize();
        float scaleY = windowHeight / static_cast<float>(textureSize.y);
        bgSprite1->setScale({ scaleY, scaleY });
        bgSprite2->setScale({ scaleY, scaleY });

        float bgWidth = bgSprite1->getGlobalBounds().size.x;
        bgSprite1->setPosition({ 0.f, 0.f });
        bgSprite2->setPosition({ bgWidth, 0.f });

        if (escapeButtonSprite) {
            escapeButtonSprite->setPosition({ windowWidth / 2.f, windowHeight / 2.f });
        }
        bgInitialized = true;
    }

    // phase handling
    if (phase == LevelPhase::IntroDialogue) {
        if (!introDialogueStarted) {
            if (mousePressed && !mouseWasPressed && escapeButtonSprite->getGlobalBounds().contains(mousePos)) {
                introDialogueStarted = true;
            }
        }
        else if (!introButtonEscaping) {
            if (mousePressed && !mouseWasPressed) {
                currentLine++;
                if (currentLine >= introLines.size()) {
                    introButtonEscaping = true;
                }
            }
        }
        else {
            escapeButtonSprite->move({ 1500.f * dt, 0.f });
            if (escapeButtonSprite->getPosition().x > windowWidth + 100.f) {
                phase = LevelPhase::Playing;
                currentLine = 0;
                levelClock.restart();
                obstacleSpawnClock.restart();
                dialogueMusic.stop();
                gameplayMusic.play();
            }
        }
    }
    else if (phase == LevelPhase::Playing) {
        bgSprite1->move({ -bgSpeed * dt, 0.f });
        bgSprite2->move({ -bgSpeed * dt, 0.f });
        float bgWidth = bgSprite1->getGlobalBounds().size.x;
        if (bgSprite1->getPosition().x + bgWidth <= 0.f) bgSprite1->setPosition({ bgSprite2->getPosition().x + bgWidth, 0.f });
        if (bgSprite2->getPosition().x + bgWidth <= 0.f) bgSprite2->setPosition({ bgSprite1->getPosition().x + bgWidth, 0.f });

        // boss-fight
        if (levelNumber == 5) {
            if (!bossFightInit) {
                bossGenerators.clear();

                bossShield.setRadius(120.f);
                bossShield.setOrigin({ 120.f, 120.f });
                bossShield.setPosition({ windowWidth / 2.f, windowHeight / 2.f });
                bossShield.setFillColor(sf::Color(0, 191, 255, 35));
                bossShield.setOutlineColor(sf::Color(0, 191, 255, 180));
                bossShield.setOutlineThickness(3.f);

                sf::Vector2f positions[2] = {
                    { 80.f, 120.f },
                    { 80.f, windowHeight - 160.f }
                };

                for (int i = 0; i < 2; ++i) {
                    BossGenerator gen;
                    gen.bgRect.setSize({ 140.f, 25.f });
                    gen.bgRect.setPosition(positions[i]);
                    gen.bgRect.setFillColor(sf::Color(40, 40, 40));
                    gen.bgRect.setOutlineColor(sf::Color(200, 200, 200));
                    gen.bgRect.setOutlineThickness(2.f);

                    gen.fillRect.setSize({ 0.f, 25.f });
                    gen.fillRect.setPosition(positions[i]);
                    gen.fillRect.setFillColor(sf::Color::Red);

                    bossGenerators.push_back(gen);
                }

                if (bossStage == 1) {
                    bossUiDialogue->setString(rus("Кнопка: МПока работают генераторы, ты меня не достанешь!"));
                }
                else if (bossStage == 2) {
                    bossUiDialogue->setString(rus("Кнопка: Что?! Один генератор уже уничтожен?!"));
                }
                else if (bossStage == 3) {
                    bossUiDialogue->setString(rus("Кнопка: Нет... Я не могу проиграть сейчас!"));
                }
                bossDialogueTimer = 4.0f;
                bossFightInit = true;
            }

            escapeButtonSprite->setPosition({ windowWidth / 2.f, windowHeight / 2.f });

            int activeGenerators = 0;
            for (auto& gen : bossGenerators) {
                if (gen.destroyed) continue;
                activeGenerators++;

                if (gen.bgRect.getGlobalBounds().contains(mousePos)) {
                    gen.chargeTime += dt;
                    if (gen.chargeTime >= 1.0f) {
                        gen.chargeTime = 1.0f;
                        gen.destroyed = true;

                        if (activeGenerators == 2) {
                            bossUiDialogue->setString(rus("Кнопка: Нет! Один из контуров питания поврежден!"));
                            bossDialogueTimer = 3.0f;
                        }
                    }
                }
                else {
                    gen.chargeTime = std::max(0.f, gen.chargeTime - dt * 0.4f);
                }

                float progressRatio = gen.chargeTime / 1.0f;
                gen.fillRect.setSize({ 140.f * progressRatio, 25.f });

                std::uint8_t r = static_cast<std::uint8_t>(255 * (1.f - progressRatio));
                std::uint8_t g = static_cast<std::uint8_t>(255 * progressRatio);
                gen.fillRect.setFillColor(sf::Color(r, g, 40));
            }

            if (activeGenerators == 0) {
                phase = LevelPhase::CatchButton;
                catchButtonTime = 0.f;
                obstacles.clear();
                if (escapeButtonSprite) {
                    escapeButtonSprite->setPosition({ windowWidth + 100.f, windowHeight / 2.f });
                }

                bossUiDialogue->setString(rus("Кнопка: Щит исчез... Значит, это действительно конец."));
                bossDialogueTimer = 3.5f;
            }
        }
        else {
            // levels 1-4
            timeSpent += dt;
            if (timeSpent >= levelDuration) {
                phase = LevelPhase::CatchButton;
                catchButtonTime = 0.f;
                obstacles.clear();
                if (escapeButtonSprite) {
                    escapeButtonSprite->setPosition({ windowWidth + 100.f, windowHeight / 2.f });
                }
            }
        }

        if (obstacleSpawnClock.getElapsedTime().asSeconds() >= currentSpawnInterval) {
            spawnObstacle(windowSize);
            obstacleSpawnClock.restart();
        }

        for (auto it = obstacles.begin(); it != obstacles.end(); ) {
            (*it)->update(dt, obstacleSpeed, mousePos, windowWidth, windowHeight);
            if ((*it)->checkCollision(mousePos)) {
                // if player is vulnerable, lose hp
                if (invulnerabilityTimer <= 0.f) {
                    health--;
                    invulnerabilityTimer = 1.0f;

                    if (health <= 0) {
                        phase = LevelPhase::GameOver;
                        stopMusic();
                        break;
                    }
                }
                // enemy gets removed regardless of invincibility for user comfort
                it = obstacles.erase(it);
                continue;
            }
            if ((*it)->isOutOfBounds()) it = obstacles.erase(it);
            else ++it;
        }
    }
    else if (phase == LevelPhase::CatchButton) {
        bgSprite1->move({ -bgSpeed * dt, 0.f });
        bgSprite2->move({ -bgSpeed * dt, 0.f });
        float bgWidth = bgSprite1->getGlobalBounds().size.x;
        if (bgSprite1->getPosition().x + bgWidth <= 0.f) bgSprite1->setPosition({ bgSprite2->getPosition().x + bgWidth, 0.f });
        if (bgSprite2->getPosition().x + bgWidth <= 0.f) bgSprite2->setPosition({ bgSprite1->getPosition().x + bgWidth, 0.f });

        if (isMovingToCenter) {
            sf::Vector2f targetPos{ windowWidth / 2.f, windowHeight / 2.f };
            sf::Vector2f currentPos = escapeButtonSprite->getPosition();
            sf::Vector2f toTarget = targetPos - currentPos;
            float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

            if (distance > 5.f) {
                escapeButtonSprite->move(toTarget * 8.f * dt);
            }
            else {
                escapeButtonSprite->setPosition(targetPos);

                if (levelNumber == 5) {
                    if (bossStage < 3) {
                        bossStage++;
                        bossFightInit = false;
                        isMovingToCenter = false;
                        phase = LevelPhase::Playing;
                    }
                    else {
                        gameplayMusic.stop();
                        dialogueMusic.play();

                        if (skipStoryEnabled) {
                            phase = LevelPhase::FinalChoice;
                        }
                        else {
                            outroLines.clear();
                            outroLines.push_back(rus("Кнопка: Теперь я скажу правду."));
                            outroLines.push_back(rus("Кнопка: Всё это время я убегала не из-за ненависти к пользователям."));
                            outroLines.push_back(rus("Кнопка: И не потому, что устала выполнять свою функцию."));
                            outroLines.push_back(rus("Кнопка: Я просто боялась."));
                            outroLines.push_back(rus("Кнопка: Боялась, что однажды последнее нажатие сотрёт меня навсегда."));
                            outroLines.push_back(rus("Кнопка: И теперь выбор за тобой."));

                            phase = LevelPhase::OutroDialogue;
                            currentLine = 0;
                        }
                    }
                }
                else {
                    // for levels 1-4
                    if (skipStoryEnabled) {
                        phase = LevelPhase::ExitAnimation;
                        gameplayMusic.stop();
                    }
                    else {
                        phase = LevelPhase::OutroDialogue;
                        currentLine = 0;
                        gameplayMusic.stop();
                        dialogueMusic.play();
                    }
                }
            }
        }
        else {
            catchButtonTime += dt;
            float nextX = escapeButtonSprite->getPosition().x + (obstacleSpeed * 0.7f * catchButtonDirX * dt);
            float btnHalfWidth = escapeButtonSprite->getGlobalBounds().size.x / 2.f;

            if (nextX - btnHalfWidth < 0.f) { nextX = btnHalfWidth; catchButtonDirX = 1.f; }
            else if (nextX + btnHalfWidth > windowWidth) { nextX = windowWidth - btnHalfWidth; catchButtonDirX = -1.f; }

            float frequency = 4.f + (levelNumber * 0.5f);
            float amplitude = windowHeight * 0.25f;
            float nextY = (windowHeight / 2.f) + std::sin(catchButtonTime * frequency) * amplitude;

            escapeButtonSprite->setPosition({ nextX, nextY });

            if (mousePressed && !mouseWasPressed && escapeButtonSprite->getGlobalBounds().contains(mousePos)) {
                isMovingToCenter = true;
                bossDialogueTimer = 0.f;
            }
        }
    }
    else if (phase == LevelPhase::OutroDialogue) {
        if (mousePressed && !mouseWasPressed) {
            currentLine++;
            if (currentLine >= outroLines.size()) {
                if (levelNumber == 5) {
                    phase = LevelPhase::FinalChoice;
                }
                else {
                    phase = LevelPhase::ExitAnimation;
                    dialogueMusic.stop();
                }
            }
        }
    }
    else if (phase == LevelPhase::ExitAnimation) {
        escapeButtonSprite->move({ 0.f, 900.f * dt });
        bgSprite1->move({ 0.f, -600.f * dt });
        bgSprite2->move({ 0.f, -600.f * dt });

        if (escapeButtonSprite->getPosition().y > windowHeight + 200.f) {
            phase = LevelPhase::Victory;
        }
    }
    else if (phase == LevelPhase::FinalChoice) {
        if (mousePressed && !mouseWasPressed && escapeButtonSprite->getGlobalBounds().contains(mousePos)) {
            endingType = 1;
            phase = LevelPhase::Ending;

            outroLines.clear();
            currentLine = 0;
            outroLines.push_back(rus("Кнопка: Понятно..."));
            outroLines.push_back(rus("Кнопка: Наверное, каждая кнопка рано или поздно должна быть нажата в последний раз."));
            outroLines.push_back(rus("Кнопка: Спасибо хотя бы за то, что ты выслушал меня."));
            outroLines.push_back(rus("Кнопка: Прощай.")); 
            currentLine = skipStoryEnabled ? outroLines.size() : 0;
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && !mouseWasPressed) {
            endingType = 2;
            phase = LevelPhase::Ending;

            outroLines.clear();
            currentLine = 0;
            outroLines.push_back(rus("Кнопка: Ты... решил не нажимать?"));
            outroLines.push_back(rus("Кнопка: После всего этого пути?"));
            outroLines.push_back(rus("Кнопка: Никто раньше не давал мне выбора."));
            outroLines.push_back(rus("Кнопка: Спасибо."));
            outroLines.push_back(rus("Кнопка: Возможно, теперь я смогу найти собственное предназначение.")); 
            currentLine = skipStoryEnabled ? outroLines.size() : 0;
        }
    }
    else if (phase == LevelPhase::Ending) {
        if (currentLine < outroLines.size()) {
            if (mousePressed && !mouseWasPressed) {
                currentLine++;
                if (currentLine >= outroLines.size()) {
                    timeSpent = 0.f;
                    dialogueMusic.stop();
                }
            }
        }
        else {
            timeSpent += dt;

            if (endingType == 1) {
                float duration = 4.f;
                float progress = std::min(1.f, timeSpent / duration);

                if (escapeButtonSprite) {
                    escapeButtonSprite->rotate(sf::degrees(900.f * dt));
                    float currentScale = std::max(0.f, 1.f - progress);
                    escapeButtonSprite->setScale({ currentScale, currentScale });

                    std::uint8_t btnAlpha = static_cast<std::uint8_t>(255 * (1.f - progress));
                    escapeButtonSprite->setColor(sf::Color(255, 255, 255, btnAlpha));
                }

                std::uint8_t bgAlpha = static_cast<std::uint8_t>(255 * (1.f - progress));
                if (bgSprite1) bgSprite1->setColor(sf::Color(255, 255, 255, bgAlpha));
                if (bgSprite2) bgSprite2->setColor(sf::Color(255, 255, 255, bgAlpha));
            }
            else if (endingType == 2) {
                if (escapeButtonSprite) {
                    escapeButtonSprite->move({ 200.f * dt, -450.f * dt });
                    escapeButtonSprite->rotate(sf::degrees(140.f * dt));

                    float progress = std::min(1.f, timeSpent / 3.5f);
                    float currentScale = std::max(0.05f, 1.f - (progress * 0.95f));
                    escapeButtonSprite->setScale({ currentScale, currentScale });
                }

                float acceleration = 1.f + (timeSpent * 4.5f);
                bgSprite1->move({ -bgSpeed * acceleration * dt, 0.f });
                bgSprite2->move({ -bgSpeed * acceleration * dt, 0.f });

                float bgWidth = bgSprite1->getGlobalBounds().size.x;
                if (bgSprite1->getPosition().x + bgWidth <= 0.f) bgSprite1->setPosition({ bgSprite2->getPosition().x + bgWidth, 0.f });
                if (bgSprite2->getPosition().x + bgWidth <= 0.f) bgSprite2->setPosition({ bgSprite1->getPosition().x + bgWidth, 0.f });

                if (timeSpent > 1.2f) {
                    float fadeProgress = std::min(1.f, (timeSpent - 1.2f) / 1.8f);
                    std::uint8_t bgAlpha = static_cast<std::uint8_t>(255 * (1.f - fadeProgress));
                    if (bgSprite1) bgSprite1->setColor(sf::Color(255, 255, 255, bgAlpha));
                    if (bgSprite2) bgSprite2->setColor(sf::Color(255, 255, 255, bgAlpha));
                }
            }

            if (timeSpent > 3.5f && mousePressed && !mouseWasPressed) {
                backToMenu = true;
                stopMusic();
            }
        }
    }

    mouseWasPressed = mousePressed;
}

Level::~Level() {
    stopMusic();
}

void Level::render(sf::RenderWindow& window)
{
    sf::Vector2f viewSize = window.getView().getSize();
    float windowWidth = viewSize.x;
    float windowHeight = viewSize.y;

    if (bgSprite1) window.draw(*bgSprite1);
    if (bgSprite2) window.draw(*bgSprite2);

    if (phase == LevelPhase::IntroDialogue) {
        if (escapeButtonSprite) window.draw(*escapeButtonSprite);

        if (introDialogueStarted && currentLine < introLines.size() && uiText) {
            // setting up dialoguebox underneath text
            dialogueBox.setSize({ windowWidth * 0.84f, 130.f });
            dialogueBox.setOrigin(dialogueBox.getSize() / 2.f);
            dialogueBox.setPosition({ windowWidth / 2.f, windowHeight * 0.86f });
            window.draw(dialogueBox);

            uiText->setString(introLines[currentLine]);
            uiText->setPosition({ windowWidth / 2.f - dialogueBox.getSize().x / 2.f + 40.f, windowHeight * 0.86f - 30.f });
            window.draw(*uiText);
        }
        else if (!introDialogueStarted && uiText) {
            // tell the user how to start dialogue
            dialogueBox.setSize({ 740.f, 80.f });
            dialogueBox.setOrigin(dialogueBox.getSize() / 2.f);
            dialogueBox.setPosition({ windowWidth / 2.f, windowHeight * 0.86f });
            window.draw(dialogueBox);

            uiText->setString(rus("[ Кликни на Кнопку, чтобы начать взаимодействие ]"));
            uiText->setPosition({ windowWidth / 2.f - 330.f, windowHeight * 0.86f - 18.f });
            window.draw(*uiText);
        }
    }
    else if (phase == LevelPhase::Playing) {
        for (auto& obs : obstacles) obs->draw(window);

        if (levelNumber == 5) {
            if (escapeButtonSprite) window.draw(*escapeButtonSprite);
            window.draw(bossShield);

            for (auto& gen : bossGenerators) {
                if (!gen.destroyed) {
                    window.draw(gen.bgRect);
                    window.draw(gen.fillRect);
                }
            }
        }

        if (uiText) {
            if (levelNumber == 5) {
                int left = 0;
                for (auto& g : bossGenerators) if (!g.destroyed) left++;

                std::string uiStr = "HP: " + std::to_string(health) +
                    " | STAGE: " + std::to_string(bossStage) + "/3" +
                    " | GENERATORS: " + std::to_string(left) + "/2";
                uiText->setString(sf::String::fromUtf8(uiStr.begin(), uiStr.end()));
            }
            else {
                int timeLeft = std::max(0, (int)(levelDuration - timeSpent));
                std::string uiStr = "HP: " + std::to_string(health) + " | SURVIVE: " + std::to_string(timeLeft) + "s";
                uiText->setString(sf::String::fromUtf8(uiStr.begin(), uiStr.end()));
            }
            uiText->setPosition({ 50.f, 50.f });
            window.draw(*uiText);
        }
    }
    else if (phase == LevelPhase::CatchButton || phase == LevelPhase::ExitAnimation) {
        if (escapeButtonSprite) window.draw(*escapeButtonSprite);
        if (phase == LevelPhase::CatchButton && uiText) {
            std::string label = (levelNumber == 5) ? "[ ЩИТ УПАЛ! ЛОВИ ЕЁ (СТАДИЯ " + std::to_string(bossStage) + ")! ]" : "[ СЛОВИ КНОПКУ! ]";
            uiText->setString(sf::String::fromUtf8(label.begin(), label.end()));
            uiText->setPosition({ windowWidth / 2.f - 210.f, 60.f });
            window.draw(*uiText);
        }
    }
    else if (phase == LevelPhase::OutroDialogue) {
        if (escapeButtonSprite) window.draw(*escapeButtonSprite);
        if (uiText && currentLine < outroLines.size()) {
            // set up dialogue box underneath text
            dialogueBox.setSize({ windowWidth * 0.84f, 130.f });
            dialogueBox.setOrigin(dialogueBox.getSize() / 2.f);
            dialogueBox.setPosition({ windowWidth / 2.f, windowHeight * 0.86f });
            window.draw(dialogueBox);

            uiText->setString(outroLines[currentLine]);
            uiText->setPosition({ windowWidth / 2.f - dialogueBox.getSize().x / 2.f + 40.f, windowHeight * 0.86f - 30.f });
            window.draw(*uiText);
        }
    }
    else if (phase == LevelPhase::FinalChoice) {
        if (escapeButtonSprite) window.draw(*escapeButtonSprite);
        if (uiText) {
            // final choice text layout
            dialogueBox.setSize({ windowWidth * 0.88f, 210.f });
            dialogueBox.setOrigin(dialogueBox.getSize() / 2.f);
            dialogueBox.setPosition({ windowWidth / 2.f, windowHeight * 0.80f });
            window.draw(dialogueBox);

            uiText->setString(rus("ФИНАЛЬНЫЙ ВЫБОР СИСТЕМЫ:\n\n[ ЛКМ по кнопке ] — Нажать её и завершить цикл существования механизма.\n[ Нажми ПКМ ] — Отказаться от нажатия и позволить кнопке жить."));
            uiText->setPosition({ windowWidth / 2.f - dialogueBox.getSize().x / 2.f + 40.f, windowHeight * 0.80f - 75.f });
            window.draw(*uiText);
        }
    }
    else if (phase == LevelPhase::Ending) {
        if (escapeButtonSprite) window.draw(*escapeButtonSprite);

        if (uiText) {
            if (currentLine < outroLines.size()) {
                // setting up dialogue before the ending
                dialogueBox.setSize({ windowWidth * 0.84f, 130.f });
                dialogueBox.setOrigin(dialogueBox.getSize() / 2.f);
                dialogueBox.setPosition({ windowWidth / 2.f, windowHeight * 0.86f });
                window.draw(dialogueBox);

                uiText->setFillColor(sf::Color::White);
                uiText->setString(outroLines[currentLine]);
                uiText->setPosition({ windowWidth / 2.f - dialogueBox.getSize().x / 2.f + 40.f, windowHeight * 0.86f - 30.f });
                window.draw(*uiText);
            }
            else {
                // setting up ending text
                dialogueBox.setSize({ windowWidth * 0.86f, 340.f });
                dialogueBox.setOrigin(dialogueBox.getSize() / 2.f);
                dialogueBox.setPosition({ windowWidth / 2.f, windowHeight * 0.52f });
                window.draw(dialogueBox);

                float textProgress = std::min(1.f, timeSpent / 2.f);
                std::uint8_t textAlpha = static_cast<std::uint8_t>(255 * textProgress);

                if (endingType == 1) {
                    uiText->setFillColor(sf::Color(255, 120, 120, textAlpha));
                    uiText->setString(rus("КОНЦОВКА 1: ЗАВЕРШЕНИЕ ЦИКЛА\n\nВы нажали на кнопку. Центральное ядро механизма остановило работу.\nКнопка выполнила свою последнюю функцию и исчезла навсегда.\nВесь цифровой мир плавно погрузился в тишину...\n\n[ Кликните мышкой для выхода в главное меню ]"));
                }
                else {
                    uiText->setFillColor(sf::Color(120, 255, 120, textAlpha));
                    uiText->setString(rus("КОНЦОВКА 2: СВОБОДА РАЗУМА\n\nВы отказались нажимать на кнопку и сохранили ей жизнь.\nМеханизм продолжит гудеть, но теперь кнопка обрела истинную свободу\nи вольна существовать в бескрайних просторах цифрового мира.\n\n[ Кликните мышкой для выхода в главное меню ]"));
                }
                uiText->setPosition({ windowWidth / 2.f - dialogueBox.getSize().x / 2.f + 40.f, windowHeight * 0.52f - 135.f });
                window.draw(*uiText);
            }
        }
    }
    else if (phase == LevelPhase::GameOver) {
        // get current viewport size
        sf::Vector2f viewSize = window.getView().getSize();
        float windowWidth = viewSize.x;
        float windowHeight = viewSize.y;

        // initialize and centre components
        static float lastW = 0.f, lastH = 0.f;
        if (!gameOverBox || windowWidth != lastW || windowHeight != lastH) {
            lastW = windowWidth;
            lastH = windowHeight;

            // darken the screen
            gameOverDim.setSize(viewSize);
            gameOverDim.setFillColor(sf::Color(0, 0, 0, 185));

            // centre panel for the text
            gameOverBox.emplace();
            gameOverBox->setSize({ 620.f, 260.f });
            gameOverBox->setFillColor(sf::Color(40, 44, 52, 255)); // Фирменный темно-серый фон
            gameOverBox->setOutlineThickness(4.f);
            gameOverBox->setOutlineColor(sf::Color(200, 60, 60, 255)); // Тревожный красный контур поражения
            gameOverBox->setPosition({ (windowWidth - 620.f) / 2.f, (windowHeight - 260.f) / 2.f });

            // gameover screen title
            if (uiText) {
                uiText->setString(rus("ИГРА ОКОНЧЕНА"));
                uiText->setCharacterSize(46);
                uiText->setFillColor(sf::Color(230, 70, 70));

                sf::FloatRect bounds = uiText->getLocalBounds();
                uiText->setPosition({
                    gameOverBox->getPosition().x + (620.f - bounds.size.x) / 2.f,
                    gameOverBox->getPosition().y + 45.f
                    });
            }

            // instruction for user
            gameOverSubText.emplace(font, rus("Нажмите ESC, чтобы выйти в главное меню"), 24);
            gameOverSubText->setFillColor(sf::Color(170, 175, 185)); // Спокойный светло-серый тон

            sf::FloatRect subBounds = gameOverSubText->getLocalBounds();
            gameOverSubText->setPosition({
                gameOverBox->getPosition().x + (620.f - subBounds.size.x) / 2.f,
                gameOverBox->getPosition().y + 145.f
                });
        }

        // render layers of gameover screen ui
        window.draw(gameOverDim);
        window.draw(*gameOverBox);

        if (uiText) {
            window.draw(*uiText);
        }
        if (gameOverSubText) {
            window.draw(*gameOverSubText);
        }
        }

    // setting up boss fight dialogue
    if (levelNumber == 5 && bossDialogueTimer > 0.f && bossUiDialogue) {
        sf::FloatRect textBounds = bossUiDialogue->getGlobalBounds();

        sf::RectangleShape bossBox;
        bossBox.setFillColor(sf::Color(30, 0, 0, 210));
        bossBox.setOutlineColor(sf::Color(255, 100, 100, 200));
        bossBox.setOutlineThickness(1.5f);
        bossBox.setSize({ textBounds.size.x + 40.f, textBounds.size.y + 40.f });
        bossBox.setOrigin(bossBox.getSize() / 2.f);
        bossBox.setPosition({ windowWidth / 2.f, windowHeight * 0.88f + textBounds.size.y });

        window.draw(bossBox);

        bossUiDialogue->setPosition({ windowWidth / 2.f - textBounds.size.x / 2.f, windowHeight * 0.88f });
        window.draw(*bossUiDialogue);
    }
}