#ifndef SBURB_SPRITE_H
#define SBURB_SPRITE_H
#include <SFML/Graphics.hpp>

#include "Common.h"
#include "Object.h"
#include "Texture.h"

namespace SBURB
{
    class Sprite : public sf::Drawable, public sf::Transformable
    {
    public:
        Sprite(std::string name, int x, int y, int width, int height, int dx, int dy, int depthing, bool collidable);
        Sprite(int texId, sf::IntRect texRect);

        sf::IntRect textureRect;
        sf::Color color;
        int textureId;

    protected:
        std::string name;
        int x;
        int y;
        int width;
        int height;
        int dx;
        int dy;
        int depthing;
        bool collidable;

    private:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    };
}

#endif