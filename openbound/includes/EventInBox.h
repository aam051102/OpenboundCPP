#ifndef SBURB_EVENT_IN_BOX_H
#define SBURB_EVENT_IN_BOX_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventInBox : public Event
    {
    public:
        EventInBox(std::string spriteName, int x, int y, int width, int height);
        ~EventInBox();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::shared_ptr<Sprite> entity;
        std::string spriteName;
        int x;
        int y;
        int width;
        int height;

    };
}
#endif
