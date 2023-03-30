#include "Sprite.h"
#include "BatchHandler.h"
#include "Serializer.h"
#include "Sburb.h"

namespace SBURB
{
    Sprite::Sprite(std::wstring name, int x, int y, int width, int height, int dx, int dy, int depthing, bool collidable)
    {
        this->name = name;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->dx = dx;
        this->dy = dy;
        this->depthing = depthing;
        this->collidable = collidable;
        this->queries = {};
        this->animations = {};
        this->animation = nullptr;
        this->actions = {};

        this->setPosition(sf::Vector2f(this->x, this->y));
    }

    void Sprite::AddAnimation(std::shared_ptr<Animation> anim) {
        if (anim == nullptr) return;
        this->animations.insert(std::pair(anim->GetName(), anim));
    }

    void Sprite::StartAnimation(std::wstring name) {
        if (this->state != name && this->animations.find(name) != this->animations.end()) {
            this->animation = this->animations[name];
            this->animation->Reset();
            this->state = name;
        }
    }

    void Sprite::Update() {
        if (this->animation) {
            if (this->animation->HasPlayed() && this->animation->GetFollowUp() != L"") {
                StartAnimation(this->animation->GetFollowUp());
            }
            else {
                this->animation->Update();
            }
        }
    }

    bool Sprite::IsBehind(std::shared_ptr<Sprite> other) {
        if (this->depthing == other->depthing) {
            return this->y + this->dy < other->y + other->dy;
        }
        else {
            return this->depthing < other->depthing;
        }
    }

    bool Sprite::Collides(std::shared_ptr<Sprite> other, float dx = 0, float dy = 0) {
        float x = this->x + dx;
        float y = this->y + dy;

        if (other->collidable) {
            if ((x - this->width / 2.0f < other->x + other->width / 2.0f) &&
                (x + this->width / 2.0f > other->x - other->width / 2.0f) &&
                (y - this->height / 2.0f < other->y + other->height / 2.0f) &&
                (y + this->height / 2.0f > other->y - other->height / 2.0f)) {
                return true;
            }
        }

        return false;
    }

    bool Sprite::HitsPoint(float x, float y) {
        if ((this->x - this->width / 2.0f <= x) &&
            (this->x + this->width / 2.0f >= x) &&
            (this->y - this->height / 2.0f <= y) &&
            (this->y + this->height / 2.0f >= y)) {

            return true;
        }

        return false;
    }

    bool Sprite::IsVisuallyUnder(int x, int y) {
        return this->animation && this->animation->IsVisuallyUnder(x - this->x, y - this->y);
    }

    void Sprite::AddAction(std::shared_ptr<Action> action) {
        this->actions.push_back(action);
    }

    void Sprite::RemoveAction(std::wstring name) {
        for (int i = 0; i < this->actions.size(); i++) {
            if (this->actions[i]->GetName() == name) {
                this->actions.erase(this->actions.begin() + i);
                break;
            }
        }
    }

    std::vector<std::shared_ptr<Action>> Sprite::GetActions(std::shared_ptr<Sprite> sprite) {
        std::vector<std::shared_ptr<Action>> validActions = {};

        for (auto action : this->actions) {
            std::wstring desired = action->GetSprite();
            if (desired == L"" || desired == sprite->GetName() || (desired[0] == L'!' && desired.substr(1) != sprite->GetName())) {
                validActions.push_back(action);
            }
        }

        return validActions;
    }

    std::map<std::wstring, sf::Vector2f> Sprite::GetBoundaryQueries(int dx = 0, int dy = 0) {
        int spriteX = this->x + dx;
        int spriteY = this->y + dy;
        float w = this->width / 2.f;
        float h = this->height / 2.f;

        this->queries[L"upRight"] = sf::Vector2f(spriteX + w, spriteY - h);
        this->queries[L"upLeft"] = sf::Vector2f(spriteX - w, spriteY - h);
        this->queries[L"downLeft"] = sf::Vector2f(spriteX - w, spriteY + h);
        this->queries[L"downRight"] = sf::Vector2f(spriteX + w, spriteY + h);
        this->queries[L"downMid"] = sf::Vector2f(spriteX, spriteY + h);
        this->queries[L"upMid"] = sf::Vector2f(spriteX, spriteY - h);

        return this->queries;
    }

    std::wstring Sprite::Serialize(std::wstring output) {
        int animationCount = 0;
        for (auto anim : this->animations) {
            animationCount++;
        }

        output = output + L"\n<sprite " +
            Serializer::SerializeAttribute(L"name", this->name) +
            Serializer::SerializeAttribute(L"x", (int)this->x) +
            Serializer::SerializeAttribute(L"y", (int)this->y) +
            Serializer::SerializeAttribute(L"dx", (int)this->dx) +
            Serializer::SerializeAttribute(L"dy", (int)this->dy) +
            Serializer::SerializeAttribute(L"width", this->width) +
            Serializer::SerializeAttribute(L"height", this->height) +
            Serializer::SerializeAttribute(L"depthing", this->depthing) +
            Serializer::SerializeAttribute(L"collidable", this->collidable) +
            (animationCount > 1 ? L"state='" + this->state + L"' " : L"") +
            L">";

        for (auto anim : this->animations) {
            output = anim.second->Serialize(output);
        }

        for (auto action : this->actions) {
            output = action->Serialize(output);
        }

        output = output + L"\n</sprite>";

        return output;
    }

    std::shared_ptr<Sprite> Sprite::Clone(std::wstring newName) {
        auto newSprite = std::make_shared<Sprite>(newName, this->x, this->y, this->width, this->height, this->dx, this->dy, this->depthing, this->collidable);

        for (auto anim : this->animations) {
            newSprite->AddAnimation(anim.second->Clone());
        }
        
        for (auto action : this->actions) {
            newSprite->AddAction(action->Clone());
        }
        
        if (this->animation) {
            newSprite->StartAnimation(this->animation->GetName());
        }
        
        Sburb::GetInstance()->SetSprite(newName, newSprite);

        return newSprite;
    }

    void Sprite::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
    {
        if (this->animation) {
            sf::RenderStates localStates = states;
            localStates.transform *= getTransform();
            target.draw(*this->animation, localStates);
        }
    }

    std::wstring Sprite::GetProp(std::wstring prop) {
        if (prop == L"name") {
            return this->name;
        }
        else if (prop == L"x") {
            return std::to_wstring((int)this->x);
        }
        else if (prop == L"y") {
            return std::to_wstring((int)this->y);
        }
        else if (prop == L"state") {
            return this->state;
        }

        return L"";
    }
}
