
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
        setCircleAlpha(pos, radius, 0);
    }

    void create(sf::Vector2i pos, int radius)
    {
        setCircleAlpha(pos, radius, 255);
    }

    sf::Image& getImage() {return m_image;}
    sf::Sprite& getSprite() {return m_sprite;}

private:
    void setCircleAlpha(sf::Vector2i pos, int radius, int alpha)
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
                    p.a = alpha;
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

    sf::Image m_image;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
};

class Player : public sf::Drawable
{
public:
    Player()
        : m_shape(sf::Vector2i(32, 32), sf::Vector2f(500, 500), sf::Color::White)
        , m_dir(0, 0)
    {
        m_shape.getSprite().setPosition(500, 500);
        m_shape.destroy(sf::Vector2i(516, 516), 32);
        m_shape.create(sf::Vector2i(516, 516), 16);
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

        m_shape.getSprite().move(m_dir.x * 100 * dt, m_dir.y * 100 * dt); //shitty but functional for the moment
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(m_shape, states);
    }

    PixelShape& getShape() {return m_shape;}

private:
    PixelShape m_shape;
    sf::Vector2f m_dir;
};

void drawCollisionIntersection(PixelShape& pixelShape1, PixelShape& pixelShape2, sf::RenderTarget& target)
{
    sf::Sprite shape1 = pixelShape1.getSprite();
    sf::Sprite shape2 = pixelShape2.getSprite();

    sf::FloatRect shapeRect1(shape1.getPosition().x, shape1.getPosition().y, shape1.getTexture()->getSize().x, shape1.getTexture()->getSize().y);
    sf::FloatRect shapeRect2(shape2.getPosition().x, shape2.getPosition().y, shape2.getTexture()->getSize().x, shape2.getTexture()->getSize().y);

    if (shapeRect1.intersects(shapeRect2))
    {
        float interLeft   = std::max(shapeRect1.left, shapeRect2.left);
        float interTop    = std::max(shapeRect1.top, shapeRect2.top);
        float interRight  = std::min(shapeRect1.left + shapeRect1.width, shapeRect2.left + shapeRect2.width);
        float interBottom = std::min(shapeRect1.top + shapeRect1.height, shapeRect2.top + shapeRect2.height);

        /* //Uncomment to view intersection relative to (0, 0) which is used for pixel checking the image
        interLeft -= shape2.getPosition().x;
        interRight -= shape2.getPosition().x;
        interTop -= shape2.getPosition().y;
        interBottom -= shape2.getPosition().y;
        */

        sf::RectangleShape rs;
        rs.setPosition(sf::Vector2f(interLeft, interTop));
        rs.setSize(sf::Vector2f(interRight - interLeft, interBottom - interTop));
        rs.setOutlineThickness(1);
        rs.setFillColor(sf::Color(255, 0, 255, 100));
        target.draw(rs);
    }

}

bool checkCollision(PixelShape& pixelShape1, PixelShape& pixelShape2)
{
    sf::Sprite shape1 = pixelShape1.getSprite();
    sf::Sprite shape2 = pixelShape2.getSprite();

    sf::FloatRect shapeRect1(shape1.getPosition().x, shape1.getPosition().y, shape1.getTexture()->getSize().x, shape1.getTexture()->getSize().y);
    sf::FloatRect shapeRect2(shape2.getPosition().x, shape2.getPosition().y, shape2.getTexture()->getSize().x, shape2.getTexture()->getSize().y);

    if (shapeRect1.intersects(shapeRect2))
    {
        float interLeft   = std::max(shapeRect1.left, shapeRect2.left);
        float interTop    = std::max(shapeRect1.top, shapeRect2.top);
        float interRight  = std::min(shapeRect1.left + shapeRect1.width, shapeRect2.left + shapeRect2.width);
        float interBottom = std::min(shapeRect1.top + shapeRect1.height, shapeRect2.top + shapeRect2.height);

        sf::FloatRect intersectRelativeRect1(interLeft - shape1.getPosition().x,
                                             interTop - shape1.getPosition().y,
                                             interRight - interLeft,
                                             interBottom - interTop);

        sf::FloatRect intersectRelativeRect2(interLeft - shape2.getPosition().x,
                                             interTop - shape2.getPosition().y,
                                             interRight - interLeft,
                                             interBottom - interTop);

        const sf::Image pixelImage1 = pixelShape1.getImage();
        const sf::Image pixelImage2 = pixelShape2.getImage();

        //width and height are the same for both intersections
        bool bitmask1[int(intersectRelativeRect1.height)+1][int(intersectRelativeRect1.width)+1];
        bool bitmask2[int(intersectRelativeRect1.height)+1][int(intersectRelativeRect1.width)+1];
        for (unsigned int y = 0; y < int(intersectRelativeRect1.height)+1; ++y)
        {
            for (unsigned int x = 0; x < int(intersectRelativeRect1.width)+1; ++x)
            {
                bitmask1[y][x] = false;
                bitmask2[y][x] = false;
            }
        }

        for (unsigned int x = intersectRelativeRect1.left; x < intersectRelativeRect1.left + intersectRelativeRect1.width; ++x)
        {
            for (unsigned int y = intersectRelativeRect1.top; y < intersectRelativeRect1.top + intersectRelativeRect1.height; ++y)
            {
                if (pixelImage1.getPixel(x, y).a > 0)
                {
                    //std::cout << int(y - intersectRelativeRect1.top) << ", " << int(x - intersectRelativeRect1.left) << "\n";
                    //std::cout << int(intersectRelativeRect1.height)+1 << ", " << int(intersectRelativeRect1.width)+1 << "\n\n";
                    bitmask1[int(y - intersectRelativeRect1.top)][int(x - intersectRelativeRect1.left)] = true;
                }
            }
        }

        for (unsigned int x = intersectRelativeRect2.left; x < intersectRelativeRect2.left + intersectRelativeRect2.width; ++x)
        {
            for (unsigned int y = intersectRelativeRect2.top; y < intersectRelativeRect2.top + intersectRelativeRect2.height; ++y)
            {
                if (pixelImage2.getPixel(x, y).a > 0)
                {
                    //std::cout << int(y - intersectRelativeRect2.top) << ", " << int(x - intersectRelativeRect2.left) << "\n";
                    //std::cout << int(intersectRelativeRect2.height)+1 << ", " << int(intersectRelativeRect2.width)+1 << "\n\n";
                    bitmask2[int(y - intersectRelativeRect2.top)][int(x - intersectRelativeRect2.left)] = true;
                }
            }
        }

        for (int y = 0; y < int(intersectRelativeRect2.height)+1; ++y)
        {
            for (int x = 0; x < int(intersectRelativeRect2.width)+1; ++x)
            {
                if (bitmask1[y][x] && bitmask2[y][x])
                {
                    return true;
                }
            }
        }
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
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        terrain1.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                        terrain2.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                        terrain3.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                        terrain4.destroy(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                    }
                    else
                    {
                        terrain1.create(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                        terrain2.create(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                        terrain3.create(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                        terrain4.create(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 50);
                    }

                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        if (!checkCollision(player.getShape(), terrain1) &&
            !checkCollision(player.getShape(), terrain2) &&
            !checkCollision(player.getShape(), terrain3) &&
            !checkCollision(player.getShape(), terrain4))
        {
            Player player2 = player;
            player2.update(prevFrameTime.asSeconds());

            if (!checkCollision(player2.getShape(), terrain1) &&
                !checkCollision(player2.getShape(), terrain2) &&
                !checkCollision(player2.getShape(), terrain3) &&
                !checkCollision(player2.getShape(), terrain4))
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

        drawCollisionIntersection(player.getShape(), terrain1, window);
        drawCollisionIntersection(player.getShape(), terrain2, window);
        drawCollisionIntersection(player.getShape(), terrain3, window);
        drawCollisionIntersection(player.getShape(), terrain4, window);

        window.display();

        //std::cout << "FPS: " << 1.f / prevFrameTime.asSeconds() << "\n";
        prevFrameTime = frameClock.restart();
    }

    return 0;
}
