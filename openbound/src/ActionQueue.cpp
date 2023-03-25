#include "ActionQueue.h"
#include <format>
#include "Sburb.h"

namespace SBURB {
    ActionQueue::ActionQueue(std::shared_ptr<Action> action, std::wstring id, std::vector<std::wstring> groups, bool noWait, bool isPaused, std::shared_ptr<Trigger> trigger) {
        this->curAction = action;

		if (id == L"") {
			this->id = std::to_wstring(Sburb::GetInstance()->GetNextQueueId() + 1);
			Sburb::GetInstance()->SetNextQueueId(Sburb::GetInstance()->GetNextQueueId() + 1);

		}
		else {
			this->id = id;
		}

        this->groups = groups;
        this->noWait = noWait;
        this->isPaused = isPaused;
        this->trigger = trigger;
    }

    ActionQueue::~ActionQueue() {

    }

	bool ActionQueue::HasGroup(std::wstring group) {
		for (int i = 0; i < this->groups.size(); i++) {
			if (this->groups[i] == group) {
				return true;
			}
		}

		return false;
	}

    std::wstring ActionQueue::Serialize(std::wstring output) {

		if (this->curAction.get() == NULL) {
			return L"";
		}

		std::wstring groupString = L"";
		for (int i = 0; i < this->groups.size(); i++) {
			groupString += ((i > 0) ? L":" : L"") + this->groups[i];
		}

		std::wstring newOutput = output + L"\n<actionQueue " + L">";
			L"id = '" + this->id +
			L"' noWait='" + std::to_wstring(this->noWait) +
			L"' paused='" + std::to_wstring(this->isPaused) + L"'" +
			(groupString.length() == 0 ? L"" : L" groups='" + groupString + L"'") +
			L">";

		newOutput = this->curAction.get()->Serialize(newOutput);
		if (this->trigger.get() != NULL) {
			newOutput = this->trigger.get()->Serialize(newOutput);
		}

		newOutput += L"</actionQueue>";
		return newOutput;
    }
}