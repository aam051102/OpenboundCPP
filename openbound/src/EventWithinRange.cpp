#include "EventWithinRange.h"
#include "Sprite.h"
#include "Game.h"

namespace SBURB {
    EventWithinRange::EventWithinRange(std::string spriteName1, std::string spriteName2, float distance) {
        this->canSerialize = false;
        this->spriteName1 = spriteName1;
        this->spriteName2 = spriteName2;
        this->distance = distance;
    }

    EventWithinRange::~EventWithinRange() {

    }

    void EventWithinRange::Reset() {

    }

    // TODO: Move to commands or something.
    std::shared_ptr<Sprite> ParseCharacterString(std::string spriteName) {
        if (spriteName == "char") {
            return Game::GetInstance()->GetCharacter();
        }
        else {
            return Game::GetInstance()->GetSprite(spriteName);
        }
    }

    bool EventWithinRange::CheckCompletion() {
        std::shared_ptr<Sprite> sprite1 = ParseCharacterString(spriteName1);
        std::shared_ptr<Sprite> sprite2 = ParseCharacterString(spriteName2);
        float xDist = sprite1->GetX() - sprite2->GetX();
        float yDist = sprite1->GetY() - sprite2->GetY();
        return sqrt(xDist * xDist + yDist * yDist) <= this->distance;
    }
}