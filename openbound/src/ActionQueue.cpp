#include "ActionQueue.h"
#include <format>

namespace SBURB {
    ActionQueue::ActionQueue(std::shared_ptr<Action> action, std::string id, std::vector<std::string> groups, bool noWait, bool isPaused, std::shared_ptr<Trigger> trigger) {
        this->curAction = action;
		this->id = id == "" ? std::to_string(nextQueueId++) : id;
        this->groups = groups;
        this->noWait = noWait;
        this->isPaused = isPaused;
        this->trigger = trigger;
    }

    ActionQueue::~ActionQueue() {

    }

	bool ActionQueue::HasGroup(std::string group) {
		for (int i = 0; i < this->groups.size(); i++) {
			if (this->groups[i] == group) {
				return true;
			}
		}

		return false;
	}

    std::string ActionQueue::Serialize(std::string output) {

		if (this->curAction.get() == NULL) {
			return "";
		}

		std::string groupString = "";
		for (int i = 0; i < this->groups.size(); i++) {
			groupString += ((i > 0) ? ":" : "") + this->groups[i];
		}

		std::string newOutput = output + "\n<actionQueue " + ">";
			"id = '" + this->id +
			"' noWait='" + std::to_string(this->noWait) +
			"' paused='" + std::to_string(this->isPaused) + "'" +
			(groupString.length() == 0 ? "" : " groups='" + groupString + "'") +
			">";

		newOutput = this->curAction.get()->Serialize(newOutput);
		if (this->trigger.get() != NULL) {
			newOutput = this->trigger.get()->Serialize(newOutput);
		}

		newOutput += "</actionQueue>";
		return newOutput;
    }
}