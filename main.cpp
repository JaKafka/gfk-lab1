#include "SFML/Graphics.hpp"
#include "ShareTechMono-Regular.h"

#include <numeric>
#include <iostream>
#include <memory>
#include <cmath>

#include "hexagon.h"
#include "rgbhex.h"
#include "cmyhex.h"
#include "hsbhex.h"
#include "hslhex.h"
#include "slider.h"

#include <future>
#include <thread>


int main()
{
    std::shared_ptr<sf::Font> font;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Jakub Kawka - GFK Lab 02", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);

    Alphabeta hexInfo;

    font = std::make_shared<sf::Font>();
    font->loadFromMemory(&(ShareTechMono_Regular_ttf[0]), ShareTechMono_Regular_ttf.size());

    sf::Clock frame_clock, around_half_secound_clock;
    sf::Int64 mean_frames_time = 0;
    std::vector<sf::Int64> frame_times;
    sf::Text text;
    text.setFont(*font);
    text.setCharacterSize(21);
    text.setFillColor(sf::Color::Black);

    Slider bar;

    hexagon_RGB h_RGB(font, hexInfo, "RGB", bar);
    hexagon_CMY h_CMY(font, hexInfo, "CMY", bar);
    hexagon_HSL h_HSL(font, hexInfo, "HSL", bar);
    hexagon_HSB h_HSB(font, hexInfo, "HSB", bar);


    text.setPosition(10, 10);
    h_RGB.Set_Borders(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(300.0f, 294.0f));
    h_CMY.Set_Borders(sf::Vector2f(300.0f, 0.0f), sf::Vector2f(600.0f, 294.0f));
    h_HSL.Set_Borders(sf::Vector2f(0.0f, 294.0f), sf::Vector2f(300.0f, 588.0f));
    h_HSB.Set_Borders(sf::Vector2f(300.0f, 294.0f), sf::Vector2f(600.0f, 588.0f));

    h_RGB.fillAlbeta();

    around_half_secound_clock.restart();

    bool beenResized;
    bool needRefresh = true;
    float sliderval = 1;



    sf::Clock sliderDelay;
    bool waitingForSlider = false;

    sf::Vector2f barLeftTop(0, 0);
    sf::Vector2f barRightBottom(0, 0);

    bool amgrabbed = false;

    sf::RenderTexture rendercache;
    rendercache.create(800, 600);
    sf::Sprite renderSprite;

    float height;
    float width;

    int minWidth = 500;
    int minHeight = 300;

    while (window.isOpen())
    {
        sf::Event event;
        window.clear(sf::Color::White);

        frame_clock.restart(); // Start pomiaru czasu.

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            // Tu trzeba obsłużyć zdarzenia: zmianę rozmiaru okna oraz klikanie w „suwaczek”.

            if (event.type == sf::Event::Resized)
            {
                beenResized = true;
                width = static_cast<float>(event.size.width);
                height = static_cast<float>(event.size.height);
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) // Handling mouse input while being resized is apparently not a good idea!
            {

                const sf::Vector2i mouse = sf::Mouse::getPosition(window);
                if (amgrabbed || (mouse.x > barLeftTop.x && mouse.x < barRightBottom.x && mouse.y > barLeftTop.y && mouse.y < barRightBottom.y))
                {
                    amgrabbed = true;
                    bar.updatePointer(mouse.y);
                    if (std::abs(bar.getValue() - sliderval) > 0.005) // :-)
                    {
                        needRefresh = true;
                        if (!waitingForSlider)
                        {
                            waitingForSlider = true;
                            sliderDelay.restart();
                        }
                    }
                }
            }
            else
                amgrabbed = false;
        }

        if (waitingForSlider && sliderDelay.getElapsedTime().asMilliseconds() > 25)
        {
            waitingForSlider = false;
        }

        if (beenResized)
        {
            if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                beenResized = false;

                bool wrongWidth = false;
                bool wrongHeight = false;

                sf::Vector2u fixVector(minWidth, minHeight);

                if (width < minWidth)
                {
                    wrongWidth = true;
                    fixVector.x = minWidth;
                }

                else if (height < minHeight)
                {
                    wrongHeight = true;
                    fixVector.y = minHeight;
                }

                if (wrongWidth || wrongHeight)
                {
                    width = minWidth;
                    height = minHeight;
                    window.setSize(fixVector);
                    beenResized = true;
                }

                rendercache.create(width, height);

                window.setView(sf::View(sf::FloatRect(0, 0, width, height)));

                // Czas na skalowanie

                const int sliderWidth = 140;                                                           // w pikselach -- ile miejsca po prawej stronie ekranu jest zarezerwowane na suwak
                const int padding = 15;                                                                // W pikselach -- ile pikesli odstępu jest od suwaka i okienka
                const float availableHeight = height - 3 * padding;                                    // Dostępna wysokość od okienek
                const float availableWidth = width - sliderWidth - 2 * padding;                        // Dostępnie horyzontalne miejsce dla heksów - padding od okienka i od slidera
                                                                                                       // Magic! It looks nice when the hex is not squashed into a square. Hexagons = bestagons
                const float hexaWidth = std::min(availableWidth / 2, (availableHeight / 2) * 0.9766f); // It'd be nice if our hex doesn't overflow
                const float hexaHeight = hexaWidth * 1.024;                                            // Square = bad

                // God bless STL
                const float interHexDistanceHorizontal = std::clamp(availableWidth - 2 * hexaWidth - padding, static_cast<float>(padding), width); // Now we need to move le hex towards le bar so le program doesn't look le ugly
                const float interHexDistanceVertical = std::clamp(availableHeight - 2 * hexaHeight - padding, static_cast<float>(padding), width);

                // Czytelność na poziomie przeciętnego projeku pisanego po kilku kawach :(
                h_RGB.Set_Borders(sf::Vector2f(padding, padding), sf::Vector2f(padding + hexaWidth, padding + hexaHeight));
                h_RGB.fillAlbeta();
                h_CMY.Set_Borders(sf::Vector2f(padding + hexaWidth + interHexDistanceHorizontal, padding), sf::Vector2f(padding + 2 * hexaWidth + interHexDistanceHorizontal, padding + hexaHeight));
                h_HSL.Set_Borders(sf::Vector2f(padding, interHexDistanceVertical + padding + hexaHeight), sf::Vector2f(padding + hexaWidth, interHexDistanceVertical + padding + 2 * hexaHeight));
                h_HSB.Set_Borders(sf::Vector2f(padding + hexaWidth + interHexDistanceHorizontal, interHexDistanceVertical + padding + hexaHeight), sf::Vector2f(padding + 2 * hexaWidth + interHexDistanceHorizontal, interHexDistanceVertical + padding + 2 * hexaHeight));

                barLeftTop.x = width - 80;
                barLeftTop.y = 20;

                barRightBottom.x = width - 30;
                barRightBottom.y = 0.75 * height;
                bar.setBorders(width - 80, width - 30, 20, 0.75 * height);

                text.setPosition(sf::Vector2f(width - 80, 0.75 * height + 30));
                needRefresh = true;
                waitingForSlider = false;
            }
        }
        else if (needRefresh && !(beenResized || waitingForSlider))
        {

            sliderval = bar.getValue();
            // Wypełnij kolorem
            auto RGBth = std::thread(&hexagon_RGB::refreshContentsAlbeta, &h_RGB);
            auto CMYth = std::thread(&hexagon_CMY::refreshContentsAlbeta, &h_CMY);
            auto HSLth = std::thread(&hexagon_HSL::refreshContentsAlbeta, &h_HSL);
            auto HSBth = std::thread(&hexagon_HSB::refreshContentsAlbeta, &h_HSB);

            // W czasie kiedy to robisz trochę porysuj
            rendercache.clear(sf::Color::White);
            rendercache.draw(bar);

            // Narysuj heksy
            RGBth.join();
            rendercache.draw(h_RGB);
            CMYth.join();
            rendercache.draw(h_CMY);
            HSLth.join();
            rendercache.draw(h_HSL);
            HSBth.join();
            rendercache.draw(h_HSB);

            rendercache.display();

            renderSprite.setTexture(rendercache.getTexture(), true);

            // Ustaw flagi
            needRefresh = false;
            waitingForSlider = false;
        }

        window.draw(renderSprite);
        text.setString(std::to_string(mean_frames_time) + "us");
        window.draw(text);

        // Pomiar czasu uśrednimy w okresie około 1/2 sekundy.
        frame_times.push_back(frame_clock.getElapsedTime().asMicroseconds());
        if (around_half_secound_clock.getElapsedTime().asSeconds() >= 0.5f && frame_times.size() >= 1)
        {
            mean_frames_time = (sf::Int64)((float)std::reduce(frame_times.begin(), frame_times.end()) / (float)frame_times.size());

            frame_times.clear();
            around_half_secound_clock.restart();
        }

        frame_clock.restart(); // Stop pomiaru czasu.
        window.display();
    }

    // Hmmm ... :-/
    font.reset();

    return 0;
}
