#ifndef SBURB_OBJECT_H
#define SBURB_OBJECT_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

namespace SBURB
{
    enum class ObjectType
    {
        Object,
        Interactable
    };

    // The abstract parent of all game objects
    class Object : public sf::Drawable, public sf::Transformable
    {
    public:
        virtual void Init() = 0;
        virtual void Update(float delta) = 0;

        int depth = 0;
        sf::IntRect collisionBox = {0, 0, 0, 0};
        ObjectType objectType = ObjectType::Object;
    };
}

#endif