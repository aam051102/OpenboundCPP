#include "Action.h"

namespace SBURB {
    Action::Action(std::string command, std::string info, std::string name, std::string sprite, std::shared_ptr<Action> followUp, bool noWait, bool noDelay, uint16_t times, bool isSoft, bool isSilent) {
        this->command = command;
        this->info = info;
        this->isSilent = isSilent;
        this->isSoft = isSoft;
        this->name = name;
        this->noWait = noWait;
        this->noDelay = noDelay;
        this->followUp = followUp;
        this->sprite = sprite;
        this->times = times;
    }

    Action::~Action() {
        
    }

    Action Action::Clone() {
        return Action(this->command, this->info, this->name, this->sprite, this->followUp, this->noWait, this->noDelay, this->times, this->isSoft, this->isSilent);
    }

    std::string Action::Serialize(std::string output) {
        std::string newOutput = output + "\n<action " +
            "command='" + this->command +
            (this->sprite != "" ? "' sprite='" + this->sprite : "") +
            (this->name != "" ? "' name='" + this->name : "") +
            (this->noWait ? "' noWait='" + this->noWait : "") +
            (this->noDelay ? "' noDelay='" + this->noDelay : "") +
            (this->isSoft ? "' soft='" + this->isSoft : "") +
            (this->isSilent ? "' silent='" + this->isSilent : "") +
            (this->times != 1 ? "' times='" + this->times : "") +
            "'>";

        newOutput += (this->info != "" ? "<args>" + this->info + "</args>" : "");

        if (this->followUp.get() != NULL) {
            newOutput = this->followUp.get()->Serialize(newOutput);
        }

        return newOutput;
    }
}