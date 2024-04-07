#pragma once

#include "SFML/Graphics.hpp"
#include "ShareTechMono-Regular.h"
#include "slider.h"

#include <numeric>
#include <iostream>
#include <memory>
#include <cmath>

// Przechowuje grafikę pixel/pixel
class pixelSprite
{
    friend class hexagon;

public:
    pixelSprite() : pixelSprite(sf::Vector2i(5, 5)) {}
    pixelSprite(const sf::Vector2i &dimensions) : size(dimensions), pixels(nullptr)
    {
        reset();
    }

    inline const sf::Sprite & getSprite() const
    {
        return sprite;
    }

    inline void setPos(const sf::Vector2f & newPos)
    {
        sprite.setPosition(newPos);
    }

    inline sf::Sprite &getSpriteForModification()
    {
        return sprite;
    }

    inline void setRGBA(int r, int g, int b, int a, int x, int y)
    {
        pixels[(y * size.x + x) * 4] = r;
        pixels[(y * size.x + x) * 4 + 1] = g;
        pixels[(y * size.x + x) * 4 + 2] = b;
        pixels[(y * size.x + x) * 4 + 3] = a;
    }

    inline int getAlpha(int x, int y)
    {
        return pixels[(y * size.x + x) * 4 + 3];
    }

    inline void refresh()
    {
        texture.create(size.x, size.y);
        texture.update(pixels);
        sprite.setTexture(texture, true);
    }

    inline void resize(const sf::Vector2i &dimensions)
    {
        size = dimensions;
        reset();
    }

    inline float getSize()
    {
        return size.x;
    }

    inline void reset()
    {
        if (pixels)
            delete pixels;
        pixels = new sf::Uint8[size.x * size.y * 4];
        for (int i = 3; i < size.x * size.y * 4; i += 4)
            pixels[i] = 0;
    }

    ~pixelSprite()
    {
        if (pixels)
            delete pixels;
    }

private:
    sf::Uint8 *pixels;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2i size;
};

// Przechowuje informację o współczynnikach i ich rombach na cele rysowania hexów
class Alphabeta
{
    friend class hexagon;

public:
    Alphabeta() : Alphabeta(sf::Vector2i(5, 5)) {}
    Alphabeta(const sf::Vector2i &dimensions) : size(dimensions), albeta(nullptr), romb(nullptr)
    {
        reset();
    }

    inline void setABR(float alpha, float beta, int rhomb, int x, int y)
    {
        albeta[(y * size.x + x) * 2] = alpha;
        albeta[(y * size.x + x) * 2 + 1] = beta;
        romb[y * size.x + x] = rhomb;
    }

    inline void getInfo(float &alpha, float &beta, int &rhomb, int x, int y) const
    {
        alpha = albeta[(y * size.x + x) * 2];
        beta = albeta[(y * size.x + x) * 2 + 1];
        rhomb = romb[y * size.x + x];
    }

    inline void resize(const sf::Vector2i &dimensions)
    {
        size = dimensions;
        reset();
    }

    inline sf::Vector2i getSize() const
    {
        return size;
    }

    inline void reset()
    {
        if (albeta)
            delete albeta;
        albeta = new float[size.x * size.y * 2];
        if (romb)
            delete romb;
        romb = new int[size.x * size.y];
        for (int i = 3; i < size.x * size.y; i++)
            romb[i] = 0;
    }

    ~Alphabeta()
    {
        if (albeta)
            delete albeta;
        if (romb)
            delete romb;
    }

private:
    sf::Vector2i size;
    float *albeta;
    int *romb;
};

class hexagon : public sf::Drawable
{

protected:
    sf::Vector2f left_top, right_bottom, center;
    sf::Vector2f p[6]; // Kolejność punktów opisana w pliku PDF do laboratorium.
    pixelSprite graphics;
    Alphabeta &albeta;
    sf::RectangleShape border;
    sf::Text title;
    sf::Text letterUp;
    sf::Text letterLeft;
    sf::Text letterRight;
    sf::ConvexShape hexa;
    std::shared_ptr<sf::Font> font;
    Slider &slide;

public:
    hexagon(const std::shared_ptr<sf::Font> &_font, Alphabeta &ab, const std::string &name, Slider &input) : slide(input), albeta(ab)
    {
        font = _font;

        border.setOutlineColor(sf::Color(200,200,200,255));
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(4);

        title = sf::Text(name, *font, 20);

        letterUp = sf::Text(name[0], *font, 20);
        letterLeft = sf::Text(name[1], *font, 20);
        letterRight = sf::Text(name[2], *font, 20);

        title.setFillColor(sf::Color::Black);
        letterUp.setFillColor(sf::Color::Black);
        letterLeft.setFillColor(sf::Color::Black);
        letterRight.setFillColor(sf::Color::Black);

        hexa = sf::ConvexShape(6);
        hexa.setOutlineThickness(4);
        hexa.setOutlineColor(sf::Color::Black);
        hexa.setFillColor(sf::Color::Transparent);
    }
    void Set_Borders(sf::Vector2f _left_top, sf::Vector2f _right_bottom);
    bool rhombus(const sf::Vector2f &p0, const sf::Vector2f &p1, const sf::Vector2f &p, float &alpha, float &beta) const;

    // Fill global alpha/beta information so that other hexes don't have to calculate this
    // Because every time you use 1000ms of CPU time a second passes!
    void fillAlbeta()
    {
        int pixels_x = std::ceil(p[5].x - p[1].x);
        int pixels_y = std::ceil(p[3].y - p[0].y);
        sf::Vector2i size(pixels_x, pixels_y); // Better safe than sorry
        albeta.resize(size);

        // Now that we have the canvas it's time fo fill it.

        // Start with the upper rhombus
        {
            const sf::Vector2f baseVector(p[1].x, p[0].y);
            const int final_y = center.y - p[0].y;
            for (int y = 0; y < final_y + 1; y++)
                for (int x = 0; x < size.x; x++)
                {
                    const sf::Vector2f currentVector(baseVector.x + x, baseVector.y + y);

                    float alpha;
                    float beta;
                    if (rhombus(center, p[5], currentVector, alpha, beta))
                    {
                        albeta.setABR(alpha, beta, 1, x, y);
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
                    if (rhombus(p[3], p[2], currentVector, alpha, beta))
                    {
                        albeta.setABR(alpha, beta, 2, x, y);
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
                    if (rhombus(p[4], p[5], currentVector, alpha, beta))
                    {
                        albeta.setABR(alpha, beta, 3, x, y);
                    }
                }
        }
    }

private:
    float d_coefficient(const sf::Vector2f &p0, const sf::Vector2f &p1, const sf::Vector2f &p) const;
};
