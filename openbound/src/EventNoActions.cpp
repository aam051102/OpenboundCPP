#include "EventNoActions.h"
#include "Game.h"
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
            return Game::GetInstance()->GetCurAction() == nullptr;
        }

        std::shared_ptr<ActionQueue> queue = Game::GetInstance()->GetActionQueueById(this->queue);
        return queue->GetCurAction() == nullptr;
    }
}