#include "Collidable.h"

namespace SBURB
{
    Collidable::Collidable(sf::IntRect collisionBox)
    {
        this->collisionBox = collisionBox;
    }

    void Collidable::Init()
    {
    }

    void Collidable::Update(float delta)
    {
    }

    void Collidable::draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
    }

}