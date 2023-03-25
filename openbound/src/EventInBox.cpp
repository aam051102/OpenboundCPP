#include "EventInBox.h"
#include "Sburb.h"

namespace SBURB {
    EventInBox::EventInBox(std::wstring spriteName, int x, int y, int width, int height) {
        this->spriteName = spriteName;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->canSerialize = false;
    }

    EventInBox::~EventInBox() {

    }

    void EventInBox::Reset() {
        if (this->spriteName == L"char") {
            this->entity = nullptr;
        }
        else {
            this->entity = Sburb::GetInstance()->GetSprite(this->spriteName);
        }
    }

    bool EventInBox::CheckCompletion() {
        auto entity = this->entity;

        if (this->spriteName == L"char") {
            entity = Sburb::GetInstance()->GetCharacter();
        }

        return entity->GetX() >= x && entity->GetY() >= y && entity->GetX() <= x + width && entity->GetY() <= y + height;
    }
}