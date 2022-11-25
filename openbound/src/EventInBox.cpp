#include "EventInBox.h"
#include "Game.h"

namespace SBURB {
    EventInBox::EventInBox(std::string spriteName, int x, int y, int width, int height) {
        this->spriteName = spriteName;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->canSerialize = true;
    }

    EventInBox::~EventInBox() {

    }

    void EventInBox::Reset() {
        if (this->spriteName == "char") {
            this->entity = nullptr;
        }
        else {
            this->entity = Game::GetInstance()->GetSprite(this->spriteName);
        }
    }

    bool EventInBox::CheckCompletion() {
        auto entity = this->entity;

        if (this->spriteName == "char") {
            entity = Game::GetInstance()->GetCharacter();
        }

        return entity->GetX() >= x && entity->GetY() >= y && entity->GetX() <= x + width && entity->GetY() <= y + height;
    }

    std::string EventInBox::Serialize() {

    }
}