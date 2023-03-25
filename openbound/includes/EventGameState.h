#ifndef SBURB_EVENT_GAME_STATE_H
#define SBURB_EVENT_GAME_STATE_H

#include "Common.h"
#include "Event.h"

namespace SBURB
{
    class EventGameState : public Event
    {
    public:
        EventGameState(std::wstring query);
        ~EventGameState();

        virtual void Reset() override;
        virtual bool CheckCompletion() override;

        bool canSerialize;

    protected:
        std::wstring token;
        std::wstring prop;
        std::wstring target;

    };
}
#endif
