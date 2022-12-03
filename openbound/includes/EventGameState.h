#ifndef SBURB_EVENT_GAME_STATE_H
#define SBURB_EVENT_GAME_STATE_H

#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventGameState : public Event
    {
    public:
        EventGameState(std::string query);
        ~EventGameState();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::string token;
        std::string prop;
        std::string target;

    };
}
#endif
