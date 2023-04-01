#include "Character.h"
#include "Sburb.h"

constexpr int FOLLOW_BUFFER_LENGTH = 6;

namespace SBURB
{
    Character::Character(std::wstring name, int x, int y, int width, int height, int sx, int sy, int sWidth, int sHeight, std::wstring sheetName, bool bootstrap) : Sprite(name, x, y, width, height, 0, 0, static_cast<int>(Depth::MG_DEPTHING), true)
    {
        this->speed = 12;
        this->vx = 0;
        this->vy = 0;
        this->facing = L"Front";
        this->npc = true;
        this->spriteType = L"character";
        this->following = nullptr;
        this->followBuffer = {};
        this->follower = nullptr;
        this->lastLeaderPos = Vector2();
        this->handledInput = -1;
        this->oldX = 0;
        this->oldY = 0;
        this->animations = {};

        if (!bootstrap)
        { // Automagically generate standard animations
            this->AddAnimation(std::make_shared<Animation>(L"idleFront", sheetName, sx, sy, sWidth, sHeight, 0, 1, L"2"));
            this->AddAnimation(std::make_shared<Animation>(L"idleRight", sheetName, sx, sy, sWidth, sHeight, 1, 1, L"2"));
            this->AddAnimation(std::make_shared<Animation>(L"idleBack", sheetName, sx, sy, sWidth, sHeight, 2, 1, L"2"));
            this->AddAnimation(std::make_shared<Animation>(L"idleLeft", sheetName, sx, sy, sWidth, sHeight, 3, 1, L"2"));
            this->AddAnimation(std::make_shared<Animation>(L"walkFront", sheetName, sx, sy, sWidth, sHeight, 4, 2, L"4"));
            this->AddAnimation(std::make_shared<Animation>(L"walkRight", sheetName, sx, sy, sWidth, sHeight, 6, 2, L"4"));
            this->AddAnimation(std::make_shared<Animation>(L"walkBack", sheetName, sx, sy, sWidth, sHeight, 8, 2, L"4"));
            this->AddAnimation(std::make_shared<Animation>(L"walkLeft", sheetName, sx, sy, sWidth, sHeight, 10, 2, L"4"));

            this->StartAnimation(L"walkFront");
        }
        else
        {
            this->bootstrap = true;
        }

        this->BecomeNPC();
    }

    void Character::Update()
    {
        this->HandleFollowing();

        // what does this code block do????
        // NOTE: These were originally this->handleInput, but the variable was never set, so I made the assumption that they were supposed to be this->handledInput. If you find issues with idling, it may be here.
        if (this->handledInput > 0)
        {
            --this->handledInput;
            if (this->handledInput == 0)
            {
                this->MoveNone();
            }
        }

        this->TryToMove(this->vx, this->vy);

        this->Sprite::Update();
    }

    void Character::HandleFollowing()
    {
        if (this->following)
        {
            if (this->following->IsNPC() && !this->IsNPC())
            {
                this->BecomeNPC();
                this->collidable = true;
                this->Walk();
            }
            else if (!this->following->IsNPC() && this->IsNPC())
            {
                this->BecomePlayer();
                this->collidable = false;
            }

            if (this->following->x != this->lastLeaderPos.x || this->following->y != this->lastLeaderPos.y)
            {
                this->followBuffer.push_back(Vector2(this->following->x, this->following->y));
                this->lastLeaderPos.x = this->following->x;
                this->lastLeaderPos.y = this->following->y;
            }

            bool didMove = false;
            Vector2 destPos;

            while (this->followBuffer.size() > FOLLOW_BUFFER_LENGTH)
            {
                destPos = this->followBuffer[0];
                didMove = true;

                std::vector<sf::Keyboard::Key> keys = {};

                sf::Vector2f delta = Sburb::GetInstance()->GetCurrentRoom()->GetInverseAdjustedMovement(this, destPos.x - this->x, destPos.y - this->y);

                if (abs(delta.x) >= this->speed / 1.9)
                {
                    if (delta.x > 0)
                    {
                        keys.push_back(sf::Keyboard::Right);
                    }
                    else
                    {
                        keys.push_back(sf::Keyboard::Left);
                    }
                }
                if (abs(delta.y) >= this->speed / 1.9)
                {
                    if (delta.y > 0)
                    {
                        keys.push_back(sf::Keyboard::Down);
                    }
                    else
                    {
                        keys.push_back(sf::Keyboard::Up);
                    }
                }
                if (keys.size() == 0)
                {
                    this->followBuffer.erase(this->followBuffer.begin() + 0);
                    continue;
                }
                else
                {
                    this->HandleInputs(keys);
                }

                break;
            }

            if (this->followBuffer.size() <= FOLLOW_BUFFER_LENGTH && !this->following->IsNPC())
            {
                if (didMove)
                {
                    this->x = destPos.x;
                    this->y = destPos.y;
                }

                this->MoveNone();
            }
        }
    }

    void Character::MoveUp(bool movingSideways)
    {
        if (!movingSideways)
        {
            this->facing = L"Back";
            this->Walk();
            this->vx = 0;
            this->vy = -this->speed;
        }
        else
        {
            this->vx *= 2.f / 3.f;
            this->vy = -this->speed * 2.f / 3.f;
        }
    }

    void Character::MoveDown(bool movingSideways)
    {
        if (!movingSideways)
        {
            this->facing = L"Front";
            this->Walk();
            this->vx = 0;
            this->vy = this->speed;
        }
        else
        {
            this->vx *= 2.f / 3.f;
            this->vy = this->speed * 2.f / 3.f;
        }
    }

    void Character::MoveLeft()
    {
        this->facing = L"Left";
        this->Walk();
        this->vx = -this->speed;
        this->vy = 0;
    }

    void Character::MoveRight()
    {
        this->facing = L"Right";
        this->Walk();
        this->vx = this->speed;
        this->vy = 0;
    }

    void Character::MoveNone()
    {
        if (this->animations[L"walkFront"]->GetFrameInterval() == 4)
        {
            this->Idle();
            this->vx = 0;
            this->vy = 0;
        }
    }

    void Character::Walk()
    {
        this->StartAnimation(L"walk" + this->facing);
    }

    void Character::Idle()
    {
        this->StartAnimation(L"idle" + this->facing);
    }

    void Character::BecomeNPC()
    {
        this->animations[L"walkFront"]->SetFrameInterval(12);
        this->animations[L"walkBack"]->SetFrameInterval(12);
        this->animations[L"walkLeft"]->SetFrameInterval(12);
        this->animations[L"walkRight"]->SetFrameInterval(12);
    }

    void Character::BecomePlayer()
    {
        this->animations[L"walkFront"]->SetFrameInterval(4);
        this->animations[L"walkBack"]->SetFrameInterval(4);
        this->animations[L"walkLeft"]->SetFrameInterval(4);
        this->animations[L"walkRight"]->SetFrameInterval(4);
    }

    void Character::HandleInputs(std::vector<sf::Keyboard::Key> order)
    {
        auto downIt = std::find(order.begin(), order.end(), sf::Keyboard::Down);
        auto upIt = std::find(order.begin(), order.end(), sf::Keyboard::Up);
        auto leftIt = std::find(order.begin(), order.end(), sf::Keyboard::Left);
        auto rightIt = std::find(order.begin(), order.end(), sf::Keyboard::Right);

        auto downAltIt = std::find(order.begin(), order.end(), sf::Keyboard::S);
        auto upAltIt = std::find(order.begin(), order.end(), sf::Keyboard::W);
        auto leftAltIt = std::find(order.begin(), order.end(), sf::Keyboard::A);
        auto rightAltIt = std::find(order.begin(), order.end(), sf::Keyboard::D);

        float down = -1;
        float up = -1;
        float left = -1;
        float right = -1;

        if (downIt != order.end())
            down = downIt - order.begin();
        else if (downAltIt != order.end())
            down = downAltIt - order.begin();

        if (upIt != order.end())
            up = upIt - order.begin();
        else if (upAltIt != order.end())
            up = upAltIt - order.begin();

        if (leftIt != order.end())
            left = leftIt - order.begin();
        else if (leftAltIt != order.end())
            left = leftAltIt - order.begin();

        if (rightIt != order.end())
            right = rightIt - order.begin();
        else if (rightAltIt != order.end())
            right = rightAltIt - order.begin();

        float none = -0.5;
        float most = std::max(std::max(left, right), none);
        bool movingSideways = true;

        if (left == most)
        {
            this->MoveLeft();
        }
        else if (right == most)
        {
            this->MoveRight();
        }
        else
        {
            movingSideways = false;
        }

        most = std::max(std::max(up, down), none);
        bool movingVertical = true;

        if (down == most)
        {
            this->MoveDown(movingSideways);
        }
        else if (up == most)
        {
            this->MoveUp(movingSideways);
        }
        else
        {
            movingVertical = false;
        }

        if (!movingSideways && !movingVertical)
        {
            this->MoveNone();
        }

        this->handledInput = 2;
    }

    bool Character::TryToMove(int vx, int vy)
    {
        // our motion could be modified somehow  - NOTE: Used for stairs and such.
        auto adjustedMovement = Sburb::GetInstance()->GetCurrentRoom()->GetAdjustedMovement(this, vx, vy);
        vx = adjustedMovement.x;
        vy = adjustedMovement.y;

        if (vx != 0 || vy != 0)
        {
            this->oldX = this->x;
            this->oldY = this->y;
        }

        std::shared_ptr<Room> room = Sburb::GetInstance()->GetCurrentRoom();

        int minX = Sburb::GetInstance()->GetScale().x; // NOTE: originally Sburb.Stage.scaleX;
        int minY = Sburb::GetInstance()->GetScale().y; // NOTE: originally Sburb.Stage.scaleY;

        while (abs(vx) >= minX || abs(vy) >= minY)
        {
            int dx = 0;
            int dy = 0;

            if (abs(vx) >= minX)
            {
                dx = round((minX)*vx / abs(vx));
                this->SetX(this->x + dx);
                vx -= dx;
            }
            if (abs(vy) >= minY)
            {
                dy = round((minY)*vy / abs(vy));
                this->SetY(this->y + dy);
                vy -= dy;
            }

            if (!this->following)
            {
                std::shared_ptr<Sprite> collision = nullptr;

                if (collision = room->Collides(this))
                {
                    bool fixed = false;
                    if (dx != 0)
                    {
                        if (!this->Collides(collision, 0, minY))
                        {
                            dy += minY;
                            this->SetY(this->y + minY);
                            fixed = true;
                        }
                        else if (!this->Collides(collision, 0, -minY))
                        {
                            dy -= minY;
                            this->SetY(this->y - minY);
                            fixed = true;
                        }
                    }
                    if (!fixed && dy != 0)
                    {
                        if (!this->Collides(collision, minX, 0))
                        {
                            dx += minX;
                            this->SetX(this->x + minX);
                            fixed = true;
                        }
                        else if (!this->Collides(collision, -minX, 0))
                        {
                            dx -= minX;
                            this->SetX(this->x - minX);
                            fixed = true;
                        }
                    }
                    if (!fixed || room->Collides(this))
                    {
                        this->SetX(this->x - dx);
                        this->SetY(this->y - dy);

                        return false;
                    }
                }

                if (!room->IsInBounds(this))
                {
                    bool fixed = false;

                    if (dx != 0)
                    {
                        if (room->IsInBounds(this, 0, minY))
                        {
                            dy += minY;
                            this->SetY(this->y + minY);
                            fixed = true;
                        }
                        else if (room->IsInBounds(this, 0, -minY))
                        {
                            dy -= minY;
                            this->SetY(this->y - minY);
                            fixed = true;
                        }
                    }

                    if (!fixed && dy != 0)
                    {
                        if (room->IsInBounds(this, minX, 0))
                        {
                            dx += minX;
                            this->SetX(this->x + minX);
                            fixed = true;
                        }
                        else if (room->IsInBounds(this, -minX, 0))
                        {
                            dx -= minX;
                            this->SetX(this->x - minX);
                            fixed = true;
                        }
                    }

                    if (!fixed || room->Collides(this))
                    {
                        this->SetX(this->x - dx);
                        this->SetY(this->y - dy);
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void Character::Follow(std::shared_ptr<Character> sprite)
    {
        while (sprite->follower != nullptr)
        {
            sprite = sprite->follower;
        }

        this->following = sprite;

        std::shared_ptr<Character> sharedThis(this);
        sprite->follower = sharedThis;

        this->followBuffer = {};
        this->lastLeaderPos = Vector2();
        this->collidable = false;
    }

    void Character::Unfollow()
    {
        if (this->following)
        {
            this->following->follower = this->follower;
            if (this->follower)
            {
                this->follower->following = this->following;
                this->follower->followBuffer = {};
            }
            this->following = nullptr;
            this->follower = nullptr;
            this->lastLeaderPos = Vector2();
            this->collidable = true;
            this->BecomeNPC();
        }
    }

    std::vector<sf::Vector2f> Character::GetActionQueries()
    {
        std::vector<sf::Vector2f> queries = {};
        queries.push_back(sf::Vector2f(this->x, this->y));

        if (this->facing == L"Front")
        {
            queries.push_back(sf::Vector2f(this->x, this->y + (this->height / 2.0f + 15)));
            queries.push_back(sf::Vector2f(this->x - this->width / 2.0f, this->y + (this->height / 2.0f + 15)));
            queries.push_back(sf::Vector2f(this->x + this->width / 2.0f, this->y + (this->height / 2.0f + 15)));
        }
        else if (this->facing == L"Back")
        {
            queries.push_back(sf::Vector2f(this->x, this->y - (this->height / 2.0f + 15)));
            queries.push_back(sf::Vector2f(this->x - this->width / 2.0f, this->y - (this->height / 2.0f + 15)));
            queries.push_back(sf::Vector2f(this->x + this->width / 2.0f, this->y - (this->height / 2.0f + 15)));
        }
        else if (this->facing == L"Right")
        {
            queries.push_back(sf::Vector2f(this->x + (this->width / 2.0f + 15), this->y));
            queries.push_back(sf::Vector2f(this->x + (this->width / 2.0f + 15), this->y + this->height / 2.0f));
            queries.push_back(sf::Vector2f(this->x + (this->width / 2.0f + 15), this->y - this->height / 2.0f));
        }
        else if (this->facing == L"Left")
        {
            queries.push_back(sf::Vector2f(this->x - (this->width / 2.0f + 15), this->y));
            queries.push_back(sf::Vector2f(this->x - (this->width / 2.0f + 15), this->y + this->height / 2.0f));
            queries.push_back(sf::Vector2f(this->x - (this->width / 2.0f + 15), this->y - this->height / 2.0f));
        }

        return queries;
    }

    std::wstring Character::Serialize(std::wstring output)
    {
        output = output + L"\n<character name='" + this->name +
            L"' x='" + std::to_wstring(this->x) +
            L"' y='" + std::to_wstring(this->y) +
            L"' width='" + std::to_wstring(this->width) +
            L"' height='" + std::to_wstring(this->height) +
            L"' state='" + this->state +
            L"' facing='" + this->facing;

        if (!this->bootstrap)
        {
            output = output + L"' sx='" + std::to_wstring(this->animations[L"walkFront"]->GetX()) +
                L"' sy='" + std::to_wstring(this->animations[L"walkFront"]->GetY()) +
                L"' sWidth='" + std::to_wstring(this->animations[L"walkFront"]->GetColSize()) +
                L"' sHeight='" + std::to_wstring(this->animations[L"walkFront"]->GetRowSize()) +
                L"' sheet='" + this->animations[L"walkFront"]->GetSheet()->GetName();
        }
        else
        {
            output = output + L"' bootstrap='true";
        }
        if (this->following)
        {
            output = output + L"' following='" + this->following->GetName() + L"";
        }
        if (this->follower)
        {
            output = output + L"' follower='" + this->follower->GetName() + L"";
        }

        output = output + L"'>";

        for (auto anim : this->animations)
        {
            if (this->bootstrap || (anim.second->GetName().find(L"idle") == std::wstring::npos && anim.second->GetName().find(L"walk") == std::wstring::npos))
            {
                output = anim.second->Serialize(output);
            }
        }

        for (auto action : this->actions)
        {
            output = action->Serialize(output);
        }

        output = output + L"\n</character>";

        return output;
    }

    bool Character::IsNPC()
    {
        return this->animations[L"walkFront"]->GetFrameInterval() == 12;
    }
}