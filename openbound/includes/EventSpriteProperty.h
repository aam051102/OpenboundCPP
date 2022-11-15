#ifndef SBURB_EVENT_SPRITE_PROPERTY_H
#define SBURB_EVENT_SPRITE_PROPERTY_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventSpriteProperty: public Event
    {
    public:
        EventSpriteProperty(std::string spriteName, std::string query);
        ~EventSpriteProperty();

        virtual void Reset() override;
        virtual std::string Serialize() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::string spriteName;
        std::string query;

    };
}
#endif
