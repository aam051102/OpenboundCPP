#ifndef SBURB_EVENT_H
#define SBURB_EVENT_H

#include <pugixml.hpp>
#include "Common.h"

namespace SBURB
{
    class Event
    {
    public:
        Event();
        ~Event();

        virtual void Reset() = 0;
        virtual std::string Serialize();
        virtual bool CheckCompletion() = 0;

        bool canSerialize;
        
    protected:

    };
}
#endif
