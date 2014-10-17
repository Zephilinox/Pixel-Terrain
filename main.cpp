
//STD
#include <iostream>
#include <memory>
#include <cmath>
#include <ctime>

//3RD
#include <SFML/Graphics.hpp>

//SELF

class PixelShape : public sf::Drawable
{
public:

    PixelShape()
    {
        m_image.create(100, 100, sf::Color::Red);
        m_texture.loadFromImage(m_image);
        m_sprite.setTexture(m_texture);
        m_sprite.setPosition(100, 100);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(m_sprite, states);
    }

    void destroy(sf::Vector2i pos, int radius)
    {
        for (int x = pos.x - radius/2; x < pos.x + radius/2; ++x)
        {
            for (int y = pos.y - radius/2; y < pos.y + radius/2; ++y)
            {
                if (x >= m_sprite.getPosition().x && x < m_sprite.getPosition().x + m_image.getSize().x &&
                    y >= m_sprite.getPosition().y && y < m_sprite.getPosition().y + m_image.getSize().y)
                {
                    sf::Color p = m_image.getPixel(x - m_sprite.getPosition().x, y - m_sprite.getPosition().y);
                    p.a = 0;
                    m_image.setPixel(x - m_sprite.getPosition().x, y - m_sprite.getPosition().y, p);
                    m_texture.loadFromImage(m_image);
                    m_sprite.setTexture(m_texture, true);
                }
            }
        }
    }

private:
    sf::Image m_image;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
};

int main()
{
    std::srand(std::time(0));

    sf::RenderWindow window(sf::VideoMode(800, 800, 32), "Pixel Terrain");
    window.setVerticalSyncEnabled(true);

    PixelShape terrain;

    sf::Event event;
    sf::Time prevFrameTime(sf::seconds(1.f / 60.f));
    sf::Clock frameClock;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                {
                    window.close();
                    break;
                }

                case sf::Event::MouseButtonPressed:
                {
                    terrain.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 10);
                }

                case sf::Event::KeyPressed:
                {
                }

                default:
                {
                    break;
                }
            }
        }


        window.clear(sf::Color(40, 40, 40));

        window.draw(terrain);

        window.display();

        std::cout << "FPS: " << 1.f / prevFrameTime.asSeconds() << "\n";
        prevFrameTime = frameClock.restart();
    }

    return 0;
}
