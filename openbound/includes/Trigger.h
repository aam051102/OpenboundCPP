#ifndef SBURB_TRIGGER_H
#define SBURB_TRIGGER_H

#include "Common.h"
#include "Action.h"
#include "Event.h"

namespace SBURB {
    class Trigger
    {
    public:
        Trigger(std::vector<std::string> info, std::shared_ptr<Action> action = NULL, std::shared_ptr<Trigger> followUp = NULL, bool shouldRestart = false, bool shouldDetonate = false, std::string op = "AND");
        ~Trigger();

        void Reset();
        bool CheckCompletion();
        bool TryToTrigger();
        std::string Serialize(std::string output);

        void SetFollowUp(std::shared_ptr<Trigger> followUp) { this->followUp = followUp; };

        
    protected:
        std::vector<std::string> info;
        std::shared_ptr<Trigger> followUp;
        std::shared_ptr<Action> action;
        bool shouldRestart;
        bool shouldDetonate;
        std::string op;
        std::shared_ptr<Trigger> waitFor;
        std::vector<Event> events;

    };
}
#endif
