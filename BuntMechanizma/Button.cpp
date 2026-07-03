#include "Button.h"

const sf::Texture& Button::getIconTexture(ButtonType type)
{
    initCache();
    return s_textureCache.at(type);
}

Button::Button(
    ButtonType type,
    const sf::Texture& texture,
    const sf::Font& font,
    const std::string& caption,
    float x,
    float y
)
    : type(type),
    sprite(texture),
    icon(getIconTexture(type)),
    text(font, caption)
{
    // Позиция подложки кнопки
    sprite.setPosition({ x, y });

    // Позиция иконки (смещение относительно кнопки)
    icon.setPosition({ x + 20.f, y + 20.f });

    // Настройка текста
    text.setCharacterSize(32);
    text.setString(sf::String::fromUtf8(caption.begin(), caption.end()));
    text.setFillColor(sf::Color::White);

    // Центрирование текста
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    sf::FloatRect textBounds = text.getLocalBounds();

    text.setPosition({
        spriteBounds.position.x + (spriteBounds.size.x) / 4.f,
        spriteBounds.position.y + (spriteBounds.size.y - textBounds.size.y) / 2.f - 12.f
        });
}

std::map<ButtonType, sf::Texture> Button::s_textureCache;

void Button::initCache() {
    if (!s_textureCache.empty()) return;

    s_textureCache[ButtonType::Play].loadFromFile("Assets/playicon_default.png");
    s_textureCache[ButtonType::Continue].loadFromFile("Assets/continueicon_default.png");
    s_textureCache[ButtonType::Help].loadFromFile("Assets/helpicon_default.png");
    s_textureCache[ButtonType::Settings].loadFromFile("Assets/settingsicon_default.png");
    s_textureCache[ButtonType::Exit].loadFromFile("Assets/exiticon_default.png");
}

void Button::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
    window.draw(icon); // Теперь здесь будет правильная текстура из кэша
    window.draw(text);
}

void Button::updateHover(sf::Vector2f mousePos)
{
    if (this->contains(mousePos))
    {
        // Эффект при наведении:
        text.setFillColor(sf::Color(255, 215, 0));  
        sprite.setColor(sf::Color(200, 200, 200));   
    }
    else
    {
        text.setFillColor(sf::Color::White);         
        sprite.setColor(sf::Color::White);          
    }
}

bool Button::contains(sf::Vector2f mousePos) const
{
    return sprite.getGlobalBounds().contains(mousePos);
}

void Button::setType(ButtonType type)
{
    auto i = s_textureCache.find(type);
    if (i != s_textureCache.end())
        icon.setTexture(i->second);
}

ButtonType Button::getType() const
{
    return type;
}