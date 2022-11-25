#include "EventPlayed.h"
#include "Game.h"

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
            this->entity = Game::GetInstance()->GetSprite(spriteName);
        }
    }

    bool EventPlayed::CheckCompletion() {
        auto entity = this->entity;

        if (this->spriteName == "char") {
            entity = Game::GetInstance()->GetCharacter();
        }

        return entity->GetAnimation()->HasPlayed();
    }
}