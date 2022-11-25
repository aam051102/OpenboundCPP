#ifndef SBURB_EVENT_PLAYED_H
#define SBURB_EVENT_PLAYED_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"
#include "Sprite.h"

namespace SBURB
{
    class EventPlayed : public Event
    {
    public:
        EventPlayed(std::string spriteName);
        ~EventPlayed();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::shared_ptr<Sprite> entity;
        std::string spriteName;

    };
}
#endif
