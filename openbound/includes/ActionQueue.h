#ifndef SBURB_ACTION_QUEUE_H
#define SBURB_ACTION_QUEUE_H

#include <pugixml.hpp>
#include "Common.h"
#include "Action.h"
#include "Trigger.h"

namespace SBURB
{
    class ActionQueue
    {
    public:
        ActionQueue(std::shared_ptr<Action> action, std::string id = "", std::vector<std::string> groups = {}, bool noWait = false, bool isPaused = false, std::shared_ptr<Trigger> trigger = NULL);
        ~ActionQueue();

        bool HasGroup(std::string group);
        std::string Serialize(std::string output);

        void SetCurrentAction(std::shared_ptr<Action> curAction) { this->curAction = curAction; }
        std::shared_ptr<Action> GetCurrentAction() { return this->curAction; };

        std::string GetId() { return this->id; };

        void SetPaused(bool isPaused) { this->isPaused = isPaused; };
        bool GetPaused() { return this->isPaused; };

        bool GetNoWait() { return this->noWait; };

        void SetTrigger(std::shared_ptr<Trigger> trigger) { this->trigger = trigger; };
        std::shared_ptr<Trigger> GetTrigger() { return this->trigger; };

    protected:
        std::shared_ptr<Action> curAction;
        std::string id;
        std::vector<std::string> groups;
        bool noWait;
        bool isPaused;
        std::shared_ptr<Trigger> trigger;

    };
}
#endif
