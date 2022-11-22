#include "Sprite.h"
#include "BatchHandler.h"

namespace SBURB
{
    Sprite::Sprite(std::string name, int x, int y, int width, int height, int dx, int dy, int depthing, bool collidable)
    {
        this->name = name;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->dx = dx;
        this->dy = dy;
        this->depthing = depthing;
        this->collidable = collidable;

        textureId = -1;
        textureRect = sf::IntRect(0, 0, 0, 0);
    }

    void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (this->animation) {
            states.transform *= getTransform();
            target.draw(*this->animation, states);
        }
    }
}
