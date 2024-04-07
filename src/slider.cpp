#include "slider.h"

void Slider::updatePointer(float mouseY)
{
    bar_position = std::clamp((mouseY - top_left.y) / rectangle.getSize().y, 0.0f, 1.0f);
    pointer.setPosition(sf::Vector2f(top_left.x, top_left.y + (bottom_right.y - top_left.y) * bar_position - 10));
    pointer_2.setPosition(sf::Vector2f(top_left.x+ 50, top_left.y + (bottom_right.y - top_left.y) * bar_position + 10));
}

void Slider::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(gradient_back, 4, sf::Quads);
    target.draw(rectangle);
    target.draw(pointer);
    target.draw(pointer_2);

}

void Slider::setBorders(float x1, float x2, float y1, float y2)
{
    top_left = sf::Vector2f(x1,y1);
    bottom_right = sf::Vector2f(x2,y2);
    rectangle.setPosition(sf::Vector2f(x1, y1));
    rectangle.setSize(sf::Vector2f(x2 - x1, y2 - y1));
    pointer.setPosition(sf::Vector2f(x1, y1 + (y2 - y1) * bar_position - 10));
    pointer_2.setPosition(sf::Vector2f(x1 + 50, y1 + (y2 - y1) * bar_position + 10));

    gradient_back[0] = sf::Vertex(sf::Vector2f(x2,y1),sf::Color::Black);
    gradient_back[1] = sf::Vertex(sf::Vector2f(x1,y1),sf::Color::Black);
    gradient_back[2] = sf::Vertex(sf::Vector2f(x1,y2),sf::Color::White);
    gradient_back[3] = sf::Vertex(sf::Vector2f(x2,y2),sf::Color::White);
}