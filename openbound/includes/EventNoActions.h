#ifndef SBURB_EVENT_NO_ACTIONS_H
#define SBURB_EVENT_NO_ACTIONS_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventNoActions: Event
    {
    public:
        EventNoActions();
        ~EventNoActions();

        virtual void Reset() override;
        virtual std::string Serialize() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:


    };
}
#endif
