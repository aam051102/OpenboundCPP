#ifndef SBURB_EVENT_MOVIE_H
#define SBURB_EVENT_MOVIE_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventMovie: public Event
    {
    public:
        EventMovie(std::wstring movieName, int threshold = 0);
        ~EventMovie();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::wstring movieName;
        int threshold;

    };
}
#endif
