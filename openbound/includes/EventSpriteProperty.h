#ifndef SBURB_EVENT_SPRITE_PROPERTY_H
#define SBURB_EVENT_SPRITE_PROPERTY_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"
#include "Sprite.h"

namespace SBURB
{
    class EventSpriteProperty: public Event
    {
    public:
        EventSpriteProperty(std::string spriteName, std::string query);
        ~EventSpriteProperty();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::shared_ptr<Sprite> entity;
        std::string spriteName;
        std::string query;

        std::string token;
        std::string prop;
        std::string target;

    };
}
#endif
