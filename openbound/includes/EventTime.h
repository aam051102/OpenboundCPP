#ifndef SBURB_EVENT_TIME_H
#define SBURB_EVENT_TIME_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventTime : public Event
    {
    public:
        EventTime(int time = 0);
        ~EventTime();

        virtual void Reset() override;
        virtual std::wstring Serialize() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        int time;
        int originalTime;

    };
}
#endif
