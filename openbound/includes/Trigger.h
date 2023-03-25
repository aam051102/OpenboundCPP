#ifndef SBURB_TRIGGER_H
#define SBURB_TRIGGER_H

#include "Common.h"
#include "Action.h"
#include "Event.h"

namespace SBURB {
    class Trigger
    {
    public:
        Trigger(std::vector<std::wstring> info, std::shared_ptr<Action> action = nullptr, std::shared_ptr<Trigger> followUp = nullptr, bool shouldRestart = false, bool shouldDetonate = false, std::wstring op = L"AND");
        ~Trigger();

        void Reset();
        bool CheckCompletion();
        bool TryToTrigger();
        std::wstring Serialize(std::wstring output);

        void SetFollowUp(std::shared_ptr<Trigger> followUp) { this->followUp = followUp; };

        void SetDetonate(bool shouldDetonate) { this->shouldDetonate = shouldDetonate; };
        
    protected:
        std::vector<std::wstring> info;
        std::shared_ptr<Trigger> followUp;
        std::shared_ptr<Action> action;
        bool shouldRestart;
        bool shouldDetonate;
        std::wstring op;
        std::shared_ptr<Trigger> waitFor;
        std::vector<std::shared_ptr<Event>> events;

    };
}
#endif
