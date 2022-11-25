#include "EventNudge.h"
#include "Input.h"

namespace SBURB {
    EventNudge::EventNudge() {
        this->canSerialize = false;
    }

    EventNudge::~EventNudge() {

    }

    void EventNudge::Reset() {

    }

    bool EventNudge::CheckCompletion() {
        return InputHandler::IsInputPressed(InputActions::Confirm) || InputHandler::IsMousePressed(MouseInputActions::Click);
    }
}