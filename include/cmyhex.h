#pragma once

#include "hexagon.h"
#include "slider.h"

class hexagon_CMY : public hexagon
{
public:
    hexagon_CMY(const std::shared_ptr<sf::Font> &_font, Alphabeta &ab, const std::string &name, Slider &input) : hexagon(_font, ab, name, input)
    {
    }
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    void refreshContentsAlbeta();
};

void hexagon_CMY::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Tu trzeba narysować sześciokąt CMY.
    // Draw the stuff
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

inline void CMYtoRGB(float c, float m, float y, int &r, int &g, int &b)
{
    r = (1 - c) * 255;
    g = (1 - m) * 255;
    b = (1 - y) * 255;
}
void hexagon_CMY::refreshContentsAlbeta()
{
    // Now that we have the canvas it's time fo fill it.
    sf::Vector2i size = albeta.getSize();
    graphics.resize(size);

    float val = slide.bar_position * 255;
    letterUp.setFillColor(sf::Color(0, val, val, 255));
    letterLeft.setFillColor(sf::Color(val, 0, val, 255));
    letterRight.setFillColor(sf::Color(val, val, 0, 255));
    float alpha;
    float beta;
    int rhomb;
    int r;
    int g;
    int b;

    for (int y = 0; y < size.y; y++)
    {
        for (int x = 0; x < size.x; x++)
        {

            albeta.getInfo(alpha, beta, rhomb, x, y);
            switch (rhomb)
            {
            case 1:
                CMYtoRGB(slide.bar_position, slide.bar_position * (1 - alpha), slide.bar_position * (1 - beta), r, g, b);
                graphics.setRGBA(r, g, b, 255, x, y);
                break;
            case 2:
                CMYtoRGB(slide.bar_position * (1 - beta), slide.bar_position, slide.bar_position * (1 - alpha), r, g, b);
                graphics.setRGBA(r, g, b, 255, x, y);
                break;
            case 3:
                CMYtoRGB(slide.bar_position * alpha, slide.bar_position * beta, slide.bar_position, r, g, b);
                graphics.setRGBA(r, g, b, 255, x, y);
                break;
            }
        }
    }

    graphics.refresh();
}
