#pragma once

#include "hexagon.h"
#include "slider.h"

class hexagon_RGB : public hexagon
{

public:
    hexagon_RGB(const std::shared_ptr<sf::Font> &_font, Alphabeta &ab, const std::string &name, Slider &input) : hexagon(_font, ab, name, input)
    {
    }
    void refreshContents();
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    void refreshContentsAlbeta();
};

void hexagon_RGB::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Draw the stuff
    // Kwadratowa ramka
    target.draw(border, states);

    // Obramowanie hexa

    // Literki
    target.draw(title);
    target.draw(letterUp);
    target.draw(letterLeft);
    target.draw(letterRight);
    target.draw(graphics.getSprite());
    target.draw(hexa);
}

void hexagon_RGB::refreshContentsAlbeta()
{

    // Now that we have the canvas it's time fo fill it.
    sf::Vector2i size = albeta.getSize();
    graphics.resize(size);
    float val = slide.bar_position * 255;
    letterUp.setFillColor(sf::Color(val, 0, 0, 255));
    letterLeft.setFillColor(sf::Color(0, val, 0, 255));
    letterRight.setFillColor(sf::Color(0, 0, val, 255));
    float alpha;
    float beta;
    int rhomb;
    for (int y = 0; y < size.y; y++)
    {
        for (int x = 0; x < size.x; x++)
        {

            albeta.getInfo(alpha, beta, rhomb, x, y);
            
            switch (rhomb)
            {
            case 1:
                graphics.setRGBA(val, val * (1 - alpha), val * (1 - beta), 255, x, y);
                break;
            case 2:
                graphics.setRGBA(val * (1 - beta), val, val * (1 - alpha), 255, x, y);
                break;
            case 3:
                graphics.setRGBA(val * alpha, val * beta, val, 255, x, y);
                break;
            }
        }
    }

    graphics.refresh();
}

void hexagon_RGB::refreshContents()
{
    int pixels_x = std::ceil(p[5].x - p[1].x);
    int pixels_y = std::ceil(p[3].y - p[0].y);
    sf::Vector2i size(pixels_x, pixels_y); // Better safe than sorry
    graphics.resize(size);

    // Now that we have the canvas it's time fo fill it.

    // Start with the upper rhombus
    {
        const sf::Vector2f baseVector(p[1].x, p[0].y);
        const int final_y = center.y - p[0].y;
        for (int y = 0; y < final_y; y++)
            for (int x = 0; x < size.x; x++)
            {
                const sf::Vector2f currentVector(baseVector.x + x, baseVector.y + y);

                float alpha;
                float beta;
                if (rhombus(center, p[5], currentVector, alpha, beta))
                {
                    graphics.setRGBA(255, (1 - alpha) * 255, (1 - beta) * 255, 255, x, y);
                }
            }
    }

    // Lower left rhombus, we ignore the part we filled already too
    // Extend the rhombus by 1 pixel up as to remove the seam effect
    {
        const int final_x = std::ceil(center.x - p[1].x);
        const int y_offset = std::floor(p[1].y - p[0].y);
        const sf::Vector2f baseVector(p[1].x, p[1].y + 1 + y_offset);
        for (int y = y_offset; y < size.y; y++)
            for (int x = 0; x < final_x; x++)
            {
                const sf::Vector2f currentVector(baseVector.x + x, baseVector.y + y);

                float alpha;
                float beta;
                if (!graphics.getAlpha(x, y) && rhombus(p[3], p[2], currentVector, alpha, beta))
                {
                    graphics.setRGBA((1 - beta) * 255, 255, (1 - alpha) * 255, 255, x, y);
                }
            }
    }

    // Lower right go brr
    {
        const int y_offset = p[1].y - p[0].y;
        const int x_offset = std::floor(center.x - p[1].x);
        const sf::Vector2f baseVector(p[0].x - x_offset, p[1].y - y_offset);
        for (int y = y_offset; y < size.y; y++)
            for (int x = x_offset; x < size.x; x++)
            {
                const sf::Vector2f currentVector(baseVector.x + x, baseVector.y + y);

                float alpha;
                float beta;
                if (!graphics.getAlpha(x, y) && rhombus(p[4], p[5], currentVector, alpha, beta))
                {
                    graphics.setRGBA(alpha * 255, beta * 255, 255, 255, x, y);
                }
            }
    }

    graphics.refresh();
}
