#include "EventNoActions.h"
#include "Sburb.h"
#include "ActionQueue.h"

namespace SBURB {
    EventNoActions::EventNoActions(std::string queue) {
        this->canSerialize = false;
        this->queue = queue;
    }

    EventNoActions::~EventNoActions() {

    }

    void EventNoActions::Reset() {

    }

    bool EventNoActions::CheckCompletion() {
        if (this->queue == "") {
            return Sburb::GetInstance()->GetQueue()->GetCurrentAction() == nullptr;
        }

        std::shared_ptr<ActionQueue> queue = Sburb::GetInstance()->GetActionQueueById(this->queue);

        return queue == nullptr || queue->GetCurrentAction() == nullptr;
    }
}