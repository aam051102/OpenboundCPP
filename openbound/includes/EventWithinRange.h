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
        EventWithinRange(std::wstring spriteName1, std::wstring spriteName2, float distance);
        ~EventWithinRange();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::wstring spriteName1;
        std::wstring spriteName2;
        float distance;

    };
}
#endif
