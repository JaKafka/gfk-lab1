#pragma once

#include "SFML/Graphics.hpp"

class Slider : public sf::Drawable
{

    friend class hexagon_HSL;
    friend class hexagon_CMY;
    friend class hexagon_HSB;
    friend class hexagon_RGB;
public:
    Slider()
    {
        rectangle.setOutlineThickness(1);
        rectangle.setOutlineColor(sf::Color::Black);
        rectangle.setFillColor(sf::Color::Transparent);

        pointer = sf::CircleShape(10, 3);
        pointer.rotate(90);
        pointer.setFillColor(sf::Color::Black);

        pointer_2 = sf::CircleShape(10, 3);
        pointer_2.setFillColor(sf::Color::Black);
        pointer_2.rotate(270);
    }
    void setBorders(float x1, float x2, float y1, float y2);
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    void updatePointer(float mouseY);
    inline float getValue() {return bar_position;}

private:
    sf::Vector2f top_left;
    sf::Vector2f bottom_right;
    sf::RectangleShape rectangle;
    sf::CircleShape pointer;
    sf::CircleShape pointer_2;
    sf::Vertex gradient_back[4];
    float bar_position = 1;
};

