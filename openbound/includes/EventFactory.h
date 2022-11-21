#ifndef SBURB_EVENT_FACTORY_H
#define SBURB_EVENT_FACTORY_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventFactory
    {
    public:
        static std::shared_ptr<Event> CreateEvent(std::string type, std::string info);

    };
}
#endif
