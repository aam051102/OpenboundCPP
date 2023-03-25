#include "EventSpriteProperty.h"
#include "Sburb.h"

namespace SBURB {
    EventSpriteProperty::EventSpriteProperty(std::wstring spriteName, std::wstring query) {
        this->canSerialize = false;
        this->spriteName = spriteName;

        this->token = L"";
        if (query.find(L">") != std::wstring::npos) this->token = L">";
        else if (query.find(L"GREATER") != std::wstring::npos) this->token = L"GREATER";
        else if (query.find(L"<") != std::wstring::npos) this->token = L"<";
        else if (query.find(L"LESS") != std::wstring::npos) this->token = L"LESS";
        else if (query.find(L"!=") != std::wstring::npos) this->token = L"!=";
        else if (query.find(L"=") != std::wstring::npos) this->token = L"=";

        auto queryParts = split(query, this->token);

        this->prop = trim(queryParts[0]);
        this->target = trim(queryParts[1]);
    }
    
    EventSpriteProperty::~EventSpriteProperty() {

    }

    void EventSpriteProperty::Reset() {
        if (this->spriteName == L"char") {
            this->entity = nullptr;
        }
        else {
            this->entity = Sburb::GetInstance()->GetSprite(this->spriteName);
        }
    }

    bool EventSpriteProperty::CheckCompletion() {
        auto entity = this->entity;

        if (this->spriteName == L"char") {
            entity = Sburb::GetInstance()->GetCharacter();
        }

        /*if (this->prop == "y") {
            std::cout << this->spriteName << ", " << token << ", " << this->prop << ": " << entity->GetProp(this->prop) << ", " << target;
        }*/

        if (token == L">" || token == L"GREATER") {
            return std::stoi(entity->GetProp(this->prop)) > std::stoi(target);
        }

        if (token == L"<" || token == L"LESS") {
            return std::stoi(entity->GetProp(this->prop)) < std::stoi(target);
        }

        if (token == L"!=") {
            return entity->GetProp(this->prop) != target;
        }

        if (token == L"=") {
            return entity->GetProp(this->prop) == target;
        }

        return false;
    }
}