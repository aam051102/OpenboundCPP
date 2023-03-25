#ifndef SBURB_EVENT_NO_ACTIONS_H
#define SBURB_EVENT_NO_ACTIONS_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventNoActions: public Event
    {
    public:
        EventNoActions(std::wstring queue = L"");
        ~EventNoActions();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::wstring queue;

    };
}
#endif
