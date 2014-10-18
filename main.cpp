
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

    PixelShape(sf::Vector2i size, sf::Vector2f pos, sf::Color c)
    {
        m_image.create(size.x, size.y, c);
        m_texture.loadFromImage(m_image);
        m_sprite.setTexture(m_texture);
        m_sprite.setPosition(pos);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(m_sprite, states);
    }

    void destroy(sf::Vector2i pos, int radius)
    {
        bool modified = false;

        for (int x = 0; x < int(m_image.getSize().x); ++x)
        {
            for (int y = 0; y < int(m_image.getSize().y); ++y)
            {
                int a = x - pos.x + m_sprite.getPosition().x;
                int b = y - pos.y + m_sprite.getPosition().y;
                if (a*a + b*b <= radius*radius)
                {
                    sf::Color p = m_image.getPixel(x, y);
                    p.a = 0;
                    m_image.setPixel(x, y, p);
                    modified = true;
                }
            }
        }

        if (modified)
        {
            m_texture.loadFromImage(m_image);
            m_sprite.setTexture(m_texture, true);
        }
    }

    const sf::Image& getImage() const {return m_image;}
    const sf::Sprite& getSprite() const {return m_sprite;}

private:
    sf::Image m_image;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
};

class Player : public sf::Drawable
{
public:
    Player()
        : m_dir(0, 0)
    {
        m_sprite.setSize(sf::Vector2f(32, 32));
        m_sprite.setOutlineThickness(1);
        m_sprite.setPosition(500, 500);
    }

    void handleEvent(const sf::Event& event)
    {
        switch (event.type)
        {
            default:
            {
                break;
            }
        }
    }

    void update(float dt)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            m_dir.y = -1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            m_dir.y = 1;
        }
        else
        {
            m_dir.y = 0;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            m_dir.x = -1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            m_dir.x = 1;
        }
        else
        {
            m_dir.x = 0;
        }

        m_sprite.move(m_dir.x * 100 * dt, m_dir.y * 100 * dt); //shitty but functional for the moment
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(m_sprite, states);
    }

    const sf::RectangleShape& getRectShape() const {return m_sprite;}

private:
    sf::RectangleShape m_sprite;
    sf::Vector2f m_dir;
};

void drawCollisionIntersection(const Player& player, const PixelShape& pixelShape, sf::RenderTarget& target)
{
    const sf::RectangleShape rect = player.getRectShape();
    const sf::Sprite shape = pixelShape.getSprite();

    sf::FloatRect plyRect(rect.getPosition().x, rect.getPosition().y, rect.getSize().x, rect.getSize().y);
    sf::FloatRect shapeRect(shape.getPosition().x, shape.getPosition().y, shape.getTexture()->getSize().x, shape.getTexture()->getSize().y);

    if (plyRect.intersects(shapeRect))
    {
        float interLeft   = std::max(plyRect.left, shapeRect.left);
        float interTop    = std::max(plyRect.top, shapeRect.top);
        float interRight  = std::min(plyRect.left + plyRect.width, shapeRect.left + shapeRect.width);
        float interBottom = std::min(plyRect.top + plyRect.height, shapeRect.top + shapeRect.height);

        /* //Uncomment to view intersection relative to (0, 0) which is used for pixel checking the image
        interLeft -= pixelShape.getSprite().getPosition().x;
        interRight -= pixelShape.getSprite().getPosition().x;
        interTop -= pixelShape.getSprite().getPosition().y;
        interBottom -= pixelShape.getSprite().getPosition().y;
        */

        sf::RectangleShape rs;
        rs.setPosition(sf::Vector2f(interLeft, interTop));
        rs.setSize(sf::Vector2f(interRight - interLeft, interBottom - interTop));
        rs.setOutlineThickness(1);
        rs.setFillColor(sf::Color(255, 0, 255));
        target.draw(rs);
    }

}

bool checkCollision(const Player& player, const PixelShape& pixelShape)
{
    const sf::RectangleShape rect = player.getRectShape();
    const sf::Sprite shape = pixelShape.getSprite();

    sf::FloatRect plyRect(rect.getPosition().x, rect.getPosition().y, rect.getSize().x, rect.getSize().y);
    sf::FloatRect shapeRect(shape.getPosition().x, shape.getPosition().y, shape.getTexture()->getSize().x, shape.getTexture()->getSize().y);

    if (plyRect.intersects(shapeRect))
    {
        float interLeft   = std::max(plyRect.left, shapeRect.left);
        float interTop    = std::max(plyRect.top, shapeRect.top);
        float interRight  = std::min(plyRect.left + plyRect.width, shapeRect.left + shapeRect.width);
        float interBottom = std::min(plyRect.top + plyRect.height, shapeRect.top + shapeRect.height);

        const sf::Image shapePixels = pixelShape.getImage();
        bool transparent = true;

        interLeft -= pixelShape.getSprite().getPosition().x; //subtract the position in order to make it relative to the pixels of the image at (0, 0)
        interRight -= pixelShape.getSprite().getPosition().x;
        interTop -= pixelShape.getSprite().getPosition().y;
        interBottom -= pixelShape.getSprite().getPosition().y;

        for (unsigned int x = interLeft; x < interRight; ++x)
        {
            for (unsigned int y = interTop; y < interBottom; ++y)
            {
                if (shapePixels.getPixel(x, y).a > 0)
                {
                    transparent = false;
                    break;
                }
            }
        }

        return !transparent;
    }

    return false;
}

int main()
{
    std::srand(std::time(0));

    sf::RenderWindow window(sf::VideoMode(800, 800, 32), "Pixel Terrain");
    window.setVerticalSyncEnabled(true);

    Player player;
    PixelShape terrain1(sf::Vector2i(200, 200), sf::Vector2f(0, 0), sf::Color::Red);
    PixelShape terrain2(sf::Vector2i(200, 200), sf::Vector2f(200, 0), sf::Color::Blue);
    PixelShape terrain3(sf::Vector2i(200, 200), sf::Vector2f(200, 200), sf::Color::Green);
    PixelShape terrain4(sf::Vector2i(200, 200), sf::Vector2f(0, 200), sf::Color::Yellow);

    sf::Event event;
    sf::Time prevFrameTime(sf::seconds(1.f / 60.f));
    sf::Clock frameClock;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            player.handleEvent(event);

            switch(event.type)
            {
                case sf::Event::Closed:
                {
                    window.close();
                    break;
                }

                case sf::Event::MouseButtonPressed:
                {
                    terrain1.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                    terrain2.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                    terrain3.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                    terrain4.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        if (!checkCollision(player, terrain1) &&
            !checkCollision(player, terrain2) &&
            !checkCollision(player, terrain3) &&
            !checkCollision(player, terrain4))
        {
            Player player2 = player;
            player2.update(prevFrameTime.asSeconds());

            if (!checkCollision(player2, terrain1) &&
                !checkCollision(player2, terrain2) &&
                !checkCollision(player2, terrain3) &&
                !checkCollision(player2, terrain4))
            {
                player = player2;
            }
        }

        window.clear(sf::Color(40, 40, 40));

        window.draw(terrain1);
        window.draw(terrain2);
        window.draw(terrain3);
        window.draw(terrain4);
        window.draw(player);
        drawCollisionIntersection(player, terrain1, window);
        drawCollisionIntersection(player, terrain2, window);
        drawCollisionIntersection(player, terrain3, window);
        drawCollisionIntersection(player, terrain4, window);

        window.display();

        //std::cout << "FPS: " << 1.f / prevFrameTime.asSeconds() << "\n";
        prevFrameTime = frameClock.restart();
    }

    return 0;
}
