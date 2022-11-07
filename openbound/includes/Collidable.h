#ifndef SBURB_COLLIDABLE_H
#define SBURB_COLLIDABLE_H

#include "Object.h"

namespace SBURB
{
    // Simple collidable class only intended for static, invisible collisions.
    class Collidable : public Object
    {
    public:
        Collidable(sf::IntRect collisionBox);

        void Init() override;
        void Update(float delta) override;

    private:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    };
}
#endif