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
        EventSpriteProperty(std::wstring spriteName, std::wstring query);
        ~EventSpriteProperty();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::shared_ptr<Sprite> entity;
        std::wstring spriteName;
        std::wstring query;

        std::wstring token;
        std::wstring prop;
        std::wstring target;

    };
}
#endif
