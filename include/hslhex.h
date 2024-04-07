#pragma once

#include "hexagon.h"
#include "slider.h"

class hexagon_HSL : public hexagon
{
public:
  hexagon_HSL(const std::shared_ptr<sf::Font> &_font, Alphabeta &ab, const std::string &name, Slider &input) : hexagon(_font, ab, name, input)
  {
    letterRight.setOutlineThickness(1);
  }
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
  void refreshContentsAlbeta();
};

void hexagon_HSL::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  // Tu trzeba narysować sześciokąt HSL.
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

inline float hue2rgb(float p, float q, float t)
{

  if (t < 0)
    t += 1;
  if (t > 1)
    t -= 1;
  if (t < 0.1667f)
    return p + (q - p) * 6 * t;
  if (t < 0.5f)
    return q;
  if (t < 0.666f)
    return p + (q - p) * (0.666f - t) * 6;

  return p;
}

// h 1-360 s 0-1, v 0-1
inline void HSLtoRGB(float h, float s, float l, int &r, int &g, int &b)
{

  if (s < 0.01)
  {
    r = g = b = l; // achromatic
  }
  else
  {
    const float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    const float p = 2 * l - q;
    r = hue2rgb(p, q, h + 0.333f) * 255;
    g = hue2rgb(p, q, h) * 255;
    b = hue2rgb(p, q, h - 0.333f) * 255;
  }
}

void hexagon_HSL::refreshContentsAlbeta()
{

  // Now that we have the canvas it's time fo fill it.
  sf::Vector2i size = albeta.getSize();
  graphics.resize(size);

  float alpha;
  float beta;
  int rhomb;

  const float val = slide.bar_position;
  

  int r;
  int g;
  int b;

  HSLtoRGB(val, 1, 0.5 , r, g, b);
  letterUp.setFillColor(sf::Color(r, g, b, 255));
  HSLtoRGB(1, val, 0.5 , r, g, b);
  letterLeft.setFillColor(sf::Color(r, g, b, 255));
  HSLtoRGB(1, 1, val , r, g, b);
  letterRight.setFillColor(sf::Color(r, g, b, 255));
   HSLtoRGB(1, 1,1- val , r, g, b);
  letterRight.setOutlineColor(sf::Color(r, g, b, 255));

  for (int y = 0; y < size.y; y++)
  {
    for (int x = 0; x < size.x; x++)
    {

      albeta.getInfo(alpha, beta, rhomb, x, y);
      switch (rhomb)
      {
      case 1:
        HSLtoRGB(1, 1 - alpha, slide.bar_position * (1 - beta), r, g, b);
        graphics.setRGBA(r, g, b, 255, x, y);
        break;
      case 2:
        HSLtoRGB((1 - beta), 1, slide.bar_position * (1 - alpha), r, g, b);
        graphics.setRGBA(r, g, b, 255, x, y);
        break;
      case 3:
        HSLtoRGB(alpha, beta, slide.bar_position, r, g, b);
        graphics.setRGBA(r, g, b, 255, x, y);
        break;
      }
    }
  }

  graphics.refresh();
}