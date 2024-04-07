#pragma once

#include "hexagon.h"
#include "hslhex.h"
#include "slider.h"

class hexagon_HSB : public hexagon
{
public:
    hexagon_HSB(const std::shared_ptr<sf::Font> &_font, Alphabeta &ab, const std::string &name, Slider &input) : hexagon(_font, ab, name, input)
    {
        letterLeft.setOutlineThickness(1);
    }
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    void refreshContentsAlbeta();
};

void hexagon_HSB::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Tu trzeba narysować sześciokąt HSB.

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

inline void hsv2rgb(float H, float S, float V, int &R, int &G, int &B)
{

    if (S < 0.001f)
    {
        R = V * 255;
        G = V * 255;
        B = V * 255;
    }
    else
    {
        const float var_h = H * 5.999f; // This is just evil
        // if (var_h == 6)  apparently H must be < than 6 buuuuut the workaround above *just works*
        //     var_h = 0;
        const int var_i = std::floor(var_h);
        const float var_1 = V * (1 - S);
        const float var_2 = V * (1 - S * (var_h - var_i));
        const float var_3 = V * (1 - S * (1 - (var_h - var_i)));

        switch (var_i)
        {
        case 0:
            R = V * 255;
            G = var_3 * 255;
            B = var_1 * 255;
            break;
        case 1:
            R = var_2 * 255;
            G = V * 255;
            B = var_1 * 255;
            break;
        case 2:
            R = var_1 * 255;
            G = V * 255;
            B = var_3 * 255;
            break;
        case 3:
            R = var_1 * 255;
            G = var_2 * 255;
            B = V * 255;
            break;
        case 4:
            R = var_3 * 255;
            G = var_1 * 255;
            B = V * 255;
            break;
        default:
            R = V * 255;
            G = var_1 * 255;
            B = var_2 * 255;
            break;
        }
    }
}

void hexagon_HSB::refreshContentsAlbeta()
{

    // Now that we have the canvas it's time fo fill it.
    sf::Vector2i size = albeta.getSize();
    graphics.resize(size);

    float alpha;
    float beta;
    int rhomb;
    int r;
    int g;
    int b;

    const float val = slide.bar_position;

    hsv2rgb(val, 1, 1, r, g, b);
    letterUp.setFillColor(sf::Color(r, g, b, 255));
    hsv2rgb(1, val, 1, r, g, b);
    letterLeft.setFillColor(sf::Color(r, g, b, 255));
    hsv2rgb(1, 1 - val, 1, r, g, b);
    letterLeft.setOutlineColor(sf::Color(r, g, b, 255));
    hsv2rgb(1, 1, val, r, g, b);
    letterRight.setFillColor(sf::Color(r, g, b, 255));

    for (int y = 0; y < size.y; y++)
    {
        for (int x = 0; x < size.x; x++)
        {

            albeta.getInfo(alpha, beta, rhomb, x, y);

            switch (rhomb)
            {
            case 1:
                hsv2rgb(1, 1 - alpha, val * (1 - beta), r, g, b);
                graphics.setRGBA(r, g, b, 255, x, y);
                break;
            case 2:
                hsv2rgb(1 - beta, 1, val * (1 - alpha), r, g, b);
                graphics.setRGBA(r, g, b, 255, x, y);
                break;
            case 3:
                hsv2rgb(alpha, beta, val, r, g, b);
                graphics.setRGBA(r, g, b, 255, x, y);
                break;
            }
        }
    }

    graphics.refresh();
}