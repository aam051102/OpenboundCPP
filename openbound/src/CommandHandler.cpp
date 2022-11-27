#include "CommandHandler.h"
#include "Sburb.h"

namespace SBURB {
    CommandHandler::CommandHandler() {
        
    }

    void CommandHandler::PerformActionSilent(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue)
    {
        action->SetTimes(action->GetTimes() - 1);

        std::string info = action->info;
        if (info != "") {
            info = trim(info);
        }

        std::string command = trim(action->GetCommand());

    }

    void CommandHandler::DisableControl(std::string info) {
        Sburb.inputDisabled = trim(info).size() > 0 ? Trigger({ info }) : true;
    }

    Trigger CommandHandler::WaitFor(std::string info) {
        CommandHandler::DisableControl(info);
        return CommandHandler::Sleep(info);
    }

    Trigger CommandHandler::Sleep(std::string info) {
        return Trigger({ info });
    }

    Trigger CommandHandler::Macro(std::string info) {
        std::vector<Action> actions = ParseActionString(info);
        Action action = actions[0];
        if (!action.GetIsSilent()) {
            action.SetIsSilent(true);
        }

        std::shared_ptr<ActionQueue> newQueue = Sburb.performAction(action);
        if (newQueue) {
            return Trigger({ "noActions," + newQueue->GetId() });
        }
    }
}