#include "hexagon.h"

// Trochę matematyki jeszcze nikomu nie zaszkodziło. Więc dwie kolejne metody to czysta matematyka.
float hexagon::d_coefficient(const sf::Vector2f &p0, const sf::Vector2f &p1, const sf::Vector2f &p) const
{
    const float A = p0.y - p1.y;
    const float B = p1.x - p0.x;
    const float C = p0.x * (p1.y - p0.y) - p0.y * (p1.x - p0.x);

    return (A * p.x + B * p.y + C) *  1 / sqrt(A * A + B * B) * 1.15470053838f;
}

bool hexagon::rhombus(const sf::Vector2f &p0, const sf::Vector2f &p1, const sf::Vector2f &p, float &alpha, float &beta) const
{
    float L = sqrt((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y));
    sf::Vector2f p3(0.5f * (p0.x - p1.x) + 0.8660254f * (p1.y - p0.y) + p0.x, 0.866025f * (p0.x - p1.x) - 0.5f * (p1.y - p0.y) + p0.y);

    beta = -d_coefficient(p0, p1, p) / L;
    alpha = d_coefficient(p0, p3, p) / L;

    if ((alpha > 1.0f) || (beta > 1.0f) || (alpha < 0.0f) || (beta < 0.0f))
        return false;
    else
        return true;
}

void hexagon::Set_Borders(sf::Vector2f _left_top, sf::Vector2f _right_bottom)
{
    left_top = _left_top;
    right_bottom = _right_bottom;
    float a = ((right_bottom.y - left_top.y) / 2.0f + 0.5f) - 20.0f;
    center = left_top + sf::Vector2f((right_bottom.x - left_top.x) / 2.0f + 0.5f, (right_bottom.y - left_top.y) / 2.0f + 0.5f + 10);

    p[0] = center - sf::Vector2f(0.0f, a);
    p[1] = center - sf::Vector2f(0.5f * sqrt(3.0f) * a, 0.5f * a);
    p[2] = center - sf::Vector2f(0.5f * sqrt(3.0f) * a, -0.5f * a);
    p[3] = center + sf::Vector2f(0.0f, a);
    p[4] = center + sf::Vector2f(0.5f * sqrt(3.0f) * a, 0.5f * a);
    p[5] = center + sf::Vector2f(0.5f * sqrt(3.0f) * a, -0.5f * a);

    border.setPosition(left_top);
    border.setSize(right_bottom - left_top);

    hexa.setPoint(0, p[0] + sf::Vector2f(0, 2));
    hexa.setPoint(1, p[1] + sf::Vector2f(1, 1));
    hexa.setPoint(2, p[2] + sf::Vector2f(1, -1));
    hexa.setPoint(3, p[3] + sf::Vector2f(0, -1));
    hexa.setPoint(4, p[4] + sf::Vector2f(-2, -1));
    hexa.setPoint(5, p[5] + sf::Vector2f(-2, 0));

    title.setPosition(left_top + sf::Vector2f(5, 5));
    sf::Vector2f textPos = p[0];
    textPos.y -= 25;
    textPos.x -= 10;
    letterUp.setPosition(textPos);

    textPos = p[2];
    textPos.x -= 15;
    letterLeft.setPosition(textPos);

    textPos = p[4];
    textPos.x += 6;
    letterRight.setPosition(textPos);

    graphics.setPos(sf::Vector2f(p[1].x, p[0].y));
}
