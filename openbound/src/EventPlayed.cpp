#include "EventPlayed.h"

namespace SBURB {
    EventPlayed::EventPlayed(std::string spriteName) {
        this->canSerialize = false;
        this->spriteName = spriteName;
        this->entity = NULL;
    }

    EventPlayed::~EventPlayed() {

    }

    void EventPlayed::Reset() {
        this->entity = Sburb.sprites[spriteName];
    }

    bool EventPlayed::CheckCompletion() {
        auto entity = this->entity;
        if (this->entity == "char") {
            entity = Sburb.character;
        }
        return entity.animation.hasPlayed();
    }
}