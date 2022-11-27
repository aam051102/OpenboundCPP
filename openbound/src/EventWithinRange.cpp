#include "EventWithinRange.h"
#include "Sprite.h"
#include "Sburb.h"
#include "Parser.h"

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

    bool EventWithinRange::CheckCompletion() {
        std::shared_ptr<Sprite> sprite1 = Parser::ParseCharacterString(spriteName1);
        std::shared_ptr<Sprite> sprite2 = Parser::ParseCharacterString(spriteName2);
        float xDist = sprite1->GetX() - sprite2->GetX();
        float yDist = sprite1->GetY() - sprite2->GetY();
        return sqrt(xDist * xDist + yDist * yDist) <= this->distance;
    }
}