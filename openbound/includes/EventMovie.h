#ifndef SBURB_EVENT_MOVIE_H
#define SBURB_EVENT_MOVIE_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventMovie: Event
    {
    public:
        EventMovie(std::string movieName);
        ~EventMovie();

        virtual void Reset() override;
        virtual std::string Serialize() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::string movieName;

    };
}
#endif
