#include "Action.h"

namespace SBURB {
    Action::Action(std::string command, std::string info, std::string name, std::string sprite, std::shared_ptr<Action> followUp, bool noWait, bool noDelay, uint16_t times, bool soft, std::string silent) {
        this->command = command;
        this->info = info;

        this->silentCause = silent;
        
        if (silent == "" || silent == "false") {
            this->silent = false;
        }
        else {
            this->silent = true;
        }
        
        this->soft = soft;
        this->name = name;
        this->noWait = noWait;
        this->noDelay = noDelay;
        this->followUp = followUp;
        this->sprite = sprite;
        this->times = times;
    }

    Action::~Action() {
        
    }

    std::shared_ptr<Action> Action::Clone() {
        return std::make_shared<Action>(this->command, this->info, this->name, this->sprite, this->followUp, this->noWait, this->noDelay, this->times, this->soft, this->silentCause);
    }

    std::string Action::Serialize(std::string output) {
        std::string newOutput = output + "\n<action " +
            "command='" + this->command +
            (this->sprite != "" ? "' sprite='" + this->sprite : "") +
            (this->name != "" ? "' name='" + this->name : "") +
            (this->noWait ? "' noWait='" + this->noWait : "") +
            (this->noDelay ? "' noDelay='" + this->noDelay : "") +
            (this->soft ? "' soft='" + this->soft : "") +
            (this->silentCause != "" ? "' silent='" + this->silentCause : "") +
            (this->times != 1 ? "' times='" + this->times : "") +
            "'>";

        newOutput += (this->info != "" ? "<args>" + this->info + "</args>" : "");

        if (this->followUp.get() != NULL) {
            newOutput = this->followUp.get()->Serialize(newOutput);
        }

        return newOutput;
    }
}