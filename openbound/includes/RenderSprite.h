#ifndef SBURB_SPRITE_H
#define SBURB_SPRITE_H

#include <SFML/Graphics.hpp>

#include "Common.h"
#include "Texture.h"

namespace SBURB
{
    class RenderSprite : public sf::Drawable, public sf::Transformable
    {
    public:
        RenderSprite();
        RenderSprite(int texId, sf::IntRect texRect);


        sf::IntRect textureRect;
        sf::Color color;
        int textureId;

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    };
}

#endif