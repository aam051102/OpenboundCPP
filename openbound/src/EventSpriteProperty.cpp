#include "EventSpriteProperty.h"
#include "Sburb.h"

namespace SBURB {
    EventSpriteProperty::EventSpriteProperty(std::string spriteName, std::string query) {
        this->canSerialize = false;
        this->spriteName = spriteName;

        this->token = "";
        if (query.find(">") != -1) this->token = ">";
        else if (query.find("GREATER") != -1) this->token = "GREATER";
        else if (query.find("<") != -1) this->token = "<";
        else if (query.find("LESS") != -1) this->token = "LESS";
        else if (query.find("!=") != -1) this->token = "!=";
        else if (query.find("=") != -1) this->token = "=";

        auto queryParts = split(query, this->token);

        this->prop = trim(queryParts[0]);
        this->target = trim(queryParts[1]);
    }
    
    EventSpriteProperty::~EventSpriteProperty() {

    }

    void EventSpriteProperty::Reset() {
        if (this->spriteName == "char") {
            this->entity = nullptr;
        }
        else {
            this->entity = Sburb::GetInstance()->GetSprite(this->spriteName);
        }
    }

    bool EventSpriteProperty::CheckCompletion() {
        auto entity = this->entity;

        if (this->spriteName == "char") {
            entity = Sburb::GetInstance()->GetCharacter();
        }

        if (token == ">" || token == "GREATER") {
            return std::stoi(entity->GetProp(this->prop)) > std::stoi(target);
        }

        if (token == "<" || token == "LESS") {
            return std::stoi(entity->GetProp(this->prop)) < std::stoi(target);
        }

        if (token == "!=") {
            return entity->GetProp(this->prop) != target;
        }

        if (token == "=") {
            return entity->GetProp(this->prop) == target;
        }
    }
}