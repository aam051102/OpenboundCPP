#include "EventPlayed.h"
#include "Sburb.h"

namespace SBURB {
    EventPlayed::EventPlayed(std::string spriteName) {
        this->canSerialize = false;
        this->spriteName = spriteName;
        this->entity = nullptr;
    }

    EventPlayed::~EventPlayed() {

    }

    void EventPlayed::Reset() {
        if (this->spriteName == "char") {
            this->entity = nullptr;
        }
        else {
            this->entity = Sburb::GetInstance()->GetSprite(spriteName);
        }
    }

    bool EventPlayed::CheckCompletion() {
        auto entity = this->entity;

        if (this->spriteName == "char") {
            entity = Sburb::GetInstance()->GetCharacter();
        }

        return entity->GetAnimation()->HasPlayed();
    }
}