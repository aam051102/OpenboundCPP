#ifndef SBURB_EVENT_NUDGE_H
#define SBURB_EVENT_NUDGE_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventNudge: public Event
    {
    public:
        EventNudge();
        ~EventNudge();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:


    };
}
#endif
