#ifndef SBURB_CHARACTER_H
#define SBURB_CHARACTER_H

#include "Common.h"
#include "Room.h"
#include "Sprite.h"
#include "Input.h"

namespace SBURB
{
    class Character : public Sprite
    {
    public:
        Character(std::wstring name, int x, int y, int width, int height, int sx, int sy, int sWidth, int sHeight, std::wstring sheetName, bool bootstrap = false);

        void Update() override;
        void HandleFollowing();

        void MoveUp(bool movingSideways = false);
        void MoveDown(bool movingSideways = false);
        void MoveLeft();
        void MoveRight();
        void MoveNone();
        void Walk();
        void Idle();

        void BecomeNPC();
        void BecomePlayer();

        void HandleInputs(std::vector<sf::Keyboard::Key> order);

        bool TryToMove(int vx, int vy);
        
        void Follow(std::shared_ptr<Character> sprite);
        void Unfollow();
        std::vector<Vector2> GetActionQueries();

        bool IsNPC();

        std::wstring Serialize(std::wstring output);

        int GetOldX() { return this->oldX; };
        int GetOldY() { return this->oldY; };

        void SetFacing(std::wstring facing) { this->facing = facing; };

        void SetFollowBuffer(std::vector<Vector2> followBuffer) { this->followBuffer = followBuffer; };

        std::shared_ptr<Character> GetFollower() { return this->follower; };

    protected:
        int speed;
        float vx;
        float vy;
        std::wstring facing;
        bool npc;
        std::wstring spriteType;
        int handledInput;
        int oldX;
        int oldY;
        bool bootstrap;
        std::shared_ptr<Character> following;
        std::vector<Vector2> followBuffer;
        std::shared_ptr<Character> follower;
        Vector2 lastLeaderPos;

    };
}
#endif
