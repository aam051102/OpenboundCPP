#ifndef SBURB_EVENT_IN_BOX_2_H
#define SBURB_EVENT_IN_BOX_2_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventInBox2 : public Event
    {
    public:
        EventInBox2(std::string spriteName, int x, int y, int width, int height);
        ~EventInBox2();

        virtual void Reset() override;
        virtual std::string Serialize() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::string spriteName;
        int x;
        int y;
        int width;
        int height;

    };
}
#endif
