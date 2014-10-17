
//STD
#include <iostream>
#include <memory>
#include <cmath>
#include <ctime>

//3RD
#include <SFML/Graphics.hpp>

//SELF

int main()
{
    std::srand(std::time(0));

    sf::RenderWindow window(sf::VideoMode(800, 800, 32), "Pixel Terrain");
    window.setVerticalSyncEnabled(true);

    sf::Image terrain;
    terrain.create(100, 100, sf::Color::Red);

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
                    for (unsigned int x = event.mouseButton.x - 10; x < event.mouseButton.x + 10; ++x)
                    {
                        for (unsigned int y = event.mouseButton.y - 10; y < event.mouseButton.y + 10; ++y)
                        {
                            sf::Color p = terrain.getPixel(x, y);
                            p.a = 0;
                            terrain.setPixel(x, y, p);
                        }
                    }

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

        sf::Texture t;
        t.loadFromImage(terrain);
        sf::Sprite s(t);
        window.draw(s);

        window.display();

        std::cout << "FPS: " << 1.f / prevFrameTime.asSeconds() << "\n";
        prevFrameTime = frameClock.restart();
    }

    return 0;
}
