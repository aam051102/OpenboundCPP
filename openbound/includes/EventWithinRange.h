#ifndef SBURB_EVENT_WITHIN_RANGE_H
#define SBURB_EVENT_WITHIN_RANGE_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventWithinRange : public Event
    {
    public:
        EventWithinRange(std::string spriteName1, std::string spriteName2, float distance);
        ~EventWithinRange();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::string spriteName1;
        std::string spriteName2;
        float distance;

    };
}
#endif
