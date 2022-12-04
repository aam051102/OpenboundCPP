#include "Sprite.h"
#include "BatchHandler.h"
#include "Serializer.h"
#include "Sburb.h"

namespace SBURB
{
    Sprite::Sprite(std::string name, int x, int y, int width, int height, int dx, int dy, int depthing, bool collidable)
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

        this->setPosition(this->x, this->y);
    }

    void Sprite::AddAnimation(std::shared_ptr<Animation> anim) {
        if (anim == nullptr) return;
        this->animations.insert(std::pair(anim->GetName(), anim));
    }

    void Sprite::StartAnimation(std::string name) {
        if (this->state != name && this->animations[name]) {
            this->animation = this->animations[name];
            this->animation->Reset();
            this->state = name;
        }
    }

    void Sprite::Update() {
        if (this->animation) {
            if (this->animation->HasPlayed() && this->animation->GetFollowUp() != "") {
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

    bool Sprite::Collides(std::shared_ptr<Sprite> other, int dx = 0, int dy = 0) {
        int x = this->x + dx;
        int y = this->y + dy;

        if (other->collidable) {
            if ((x - this->width / 2 < other->x + other->width / 2) &&
                (x + this->width / 2 > other->x - other->width / 2) &&
                (y - this->height / 2 < other->y + other->height / 2) &&
                (y + this->height / 2 > other->y - other->height / 2)) {
                return true;
            }
        }

        return false;
    }

    bool Sprite::HitsPoint(int x, int y) {
        if ((this->x - this->width / 2 <= x) &&
            (this->x + this->width / 2 >= x) &&
            (this->y - this->height / 2 <= y) &&
            (this->y + this->height / 2 >= y)) {
            return true;
        }

        return false;
    }

    bool Sprite::IsVisuallyUnder(int x, int y) {
        return this->animation && this->animation->IsVisuallyUnder(x - this->x, y - this->y);
    }

    void Sprite::AddAction(std::shared_ptr<Action> action) {
        this->actions.insert(std::pair(action->GetName(), action));
    }

    void Sprite::RemoveAction(std::string name) {
        this->actions.erase(name);
    }

    std::vector<std::shared_ptr<Action>> Sprite::GetActions(std::shared_ptr<Sprite> sprite) {
        std::vector<std::shared_ptr<Action>> validActions = {};

        for (auto action : (this->actions)) {
            std::string desired = action.second->GetSprite();
            if (desired == "" || desired == sprite->GetName()
                || (desired[0] == '!' && desired.substr(1) != sprite->GetName())) {
                validActions.push_back(action.second);
            }
        }

        return validActions;
    }

    std::map<std::string, Vector2> Sprite::GetBoundaryQueries(int dx = 0, int dy = 0) {
        int spriteX = this->x + dx;
        int spriteY = this->y + dy;
        int w = this->width / 2;
        int h = this->height / 2;

        this->queries["upRight"] = Vector2(spriteX + w, spriteY - h);
        this->queries["upLeft"] = Vector2(spriteX - w, spriteY - h);
        this->queries["downLeft"] = Vector2(spriteX - w, spriteY + h);
        this->queries["downRight"] = Vector2(spriteX + w, spriteY + h);
        this->queries["downMid"] = Vector2(spriteX, spriteY + h);
        this->queries["upMid"] = Vector2(spriteX, spriteY - h);

        return this->queries;
    }

    std::string Sprite::Serialize(std::string output) {
        int animationCount = 0;
        for (auto anim : this->animations) {
            animationCount++;
        }

        output = output + "\n<sprite " +
            Serializer::SerializeAttribute("name", this->name) +
            Serializer::SerializeAttribute("x", this->x) +
            Serializer::SerializeAttribute("y", this->y) +
            Serializer::SerializeAttribute("dx", this->dx) +
            Serializer::SerializeAttribute("dy", this->dy) +
            Serializer::SerializeAttribute("width", this->width) +
            Serializer::SerializeAttribute("height", this->height) +
            Serializer::SerializeAttribute("depthing", this->depthing) +
            Serializer::SerializeAttribute("collidable", this->collidable) +
            (animationCount > 1 ? "state='" + this->state + "' " : "") +
            ">";

        for (auto anim : this->animations) {
            output = anim.second->Serialize(output);
        }

        for (auto action : this->actions) {
            output = action.second->Serialize(output);
        }

        output = output + "\n</sprite>";

        return output;
    }

    std::shared_ptr<Sprite> Sprite::Clone(std::string newName) {
        auto newSprite = std::make_shared<Sprite>(newName, this->x, this->y, this->width, this->height, this->dx, this->dy, this->depthing, this->collidable);

        for (auto anim : this->animations) {
            newSprite->AddAnimation(anim.second->Clone());
        }
        
        for (auto action : this->actions) {
            newSprite->AddAction(action.second->Clone());
        }
        
        if (this->animation) {
            newSprite->StartAnimation(this->animation->GetName());
        }
        
        Sburb::GetInstance()->SetSprite(newName, newSprite);

        return newSprite;
    }

    void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (this->animation) {
            states.transform *= getTransform();
            target.draw(*this->animation, states);
        }
    }

    std::string Sprite::GetProp(std::string prop) {
        if (prop == "name") {
            return this->name;
        }
        else if (prop == "x") {
            return std::to_string(this->x);
        }
        else if (prop == "y") {
            return std::to_string(this->y);
        }
        else if (prop == "state") {
            return this->state;
        }
    }
}
