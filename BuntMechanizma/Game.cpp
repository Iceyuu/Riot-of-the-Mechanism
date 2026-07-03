#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <windows.h>

void Game::initVariables()
{
    this->window = nullptr;
}

void Game::initWindow()
{
    this->videoMode = sf::VideoMode::getDesktopMode();
    this->window = new sf::RenderWindow(this->videoMode, "Game", sf::Style::None);

    const sf::Image icon("Assets/button.png");
    window->setIcon(icon);

    this->fadeRect.setSize(sf::Vector2f(static_cast<float>(this->videoMode.size.x), static_cast<float>(this->videoMode.size.y)));
    this->fadeRect.setFillColor(sf::Color(0, 0, 0, 0));
}

// game constructor
Game::Game()
    : settings(),
    levelManager(settings)
{
    this->initVariables();
    this->initWindow();
    state = State::Menu;
    settings.load();
    isFadingOut = false;
    isFadingIn = false;
    fadeAlpha = 0.f;
    fadeClock.restart();

    if (clickBuffer.loadFromFile("Assets/click.wav")) {
        clickSound.emplace(clickBuffer);
        clickSound->setVolume(settings.sfxVolume);
    }

    if (menuMusic.openFromFile("Assets/menu_music.mp3")) {
        menuMusic.setLooping(true);
        menuMusic.setVolume(settings.bgmVolume);
        menuMusic.play();
    }

    menu.updateSaveStatus();
}

// game destructor
Game::~Game()
{
    menuMusic.stop();
    levelManager.stopMusic();
    delete this->window;
}

const bool Game::running() const
{
    return this->window->isOpen();
}

void Game::pollEvents()
{
    while (const std::optional event = this->window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            this->window->close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
            {
                if (state == State::Playing) {
                    state = State::Pause;
                    levelManager.pauseMusic();
                }
                else if (state == State::Pause) {
                    state = State::Playing;
                    levelManager.resumeMusic();
                }
                else if (state == State::Menu) this->window->close();
            }
        }
    }
}

void Game::update()
{
    float fadeDt = fadeClock.restart().asSeconds();
    if (fadeDt > 0.1f) fadeDt = 0.1f;

    if (isFadingOut)
    {
        fadeAlpha += 450.f * fadeDt;
        if (fadeAlpha >= 255.f)
        {
            fadeAlpha = 255.f;
            isFadingOut = false;
            isFadingIn = true;
            state = nextState;

            if (state == State::Playing) {
                menuMusic.stop();
                levelManager.startLevel();
            }
        }
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
        this->pollEvents();
        return;
    }

    if (isFadingIn)
    {
        fadeAlpha -= 450.f * fadeDt;
        if (fadeAlpha <= 0.f)
        {
            fadeAlpha = 0.f;
            isFadingIn = false;
        }
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
    }

    this->pollEvents();

    switch (state)
    {
    case State::Menu:
    {
        auto result = menu.update(*window);
        if (result.has_value())
        {
            clickSound->play();
            switch (*result)
            {
            case ButtonType::Play:
                levelManager.resetToFirstLevel();
                isFadingOut = true;
                fadeAlpha = 0.f;
                nextState = State::Playing;
                break;

            case ButtonType::Continue:
                isFadingOut = true;
                fadeAlpha = 0.f;
                nextState = State::Playing;
                break;

            case ButtonType::Settings: state = State::Settings; break;
            case ButtonType::Help: 
                ShellExecuteA(NULL, "open", "Assets\\help.chm", NULL, NULL, SW_SHOWNORMAL);
                break;
            case ButtonType::Exit: window->close(); break;
            }
        }
        break;
    }
    case State::Settings:
    {
        bool closeOverlay = false;
        settingsOverlay.update(*window, settings, closeOverlay, clickSound);
        menuMusic.setVolume(settings.bgmVolume);
        if (closeOverlay) { settings.save(); state = State::Menu; }
        break;
    }
    case State::Playing:
    {
        bool backToMenu = false;
        levelManager.update(*window, backToMenu);
        if (backToMenu) {
            state = State::Menu;
            menuMusic.setVolume(settings.bgmVolume);
            menuMusic.play();
            menu.updateSaveStatus();
        }
        break;
    }
    case State::Pause:
    {
        auto result = pauseMenu.update(*window);
        if (result.has_value())
        {
            clickSound->play();
            switch (*result)
            {
            case ButtonType::Play:
                state = State::Playing;
                levelManager.resumeMusic();
                break;
            case ButtonType::Exit:
                state = State::Menu;
                levelManager.unloadLevel();
                menuMusic.play();
                menu.updateSaveStatus();
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}

void Game::render()
{
    this->window->clear();

    switch (state)
    {
    case State::Menu: menu.render(*window); break;
    case State::Settings: menu.render(*window); settingsOverlay.render(*window, settings); break;
    case State::Playing: levelManager.render(*window); break;
    case State::Pause: levelManager.render(*window); pauseMenu.render(*window); break;
    }

    if (isFadingOut || isFadingIn)
    {
        this->window->draw(fadeRect);
    }

    this->window->display();
}