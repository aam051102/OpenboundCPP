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
        Character(std::string name, int x, int y, int width, int height, int sx, int sy, int sWidth, int sHeight, std::string sheetName, bool bootstrap = false);
        ~Character();

        void Update();
        void HandleFollowing();

        void MoveUp(bool movingSideways);
        void MoveDown(bool movingSideways);
        void MoveLeft();
        void MoveRight();
        void MoveNone();
        void Walk();
        void Idle();

        void BecomeNPC();
        void BecomePlayer();

        void HandleInputs(std::vector<InputActions> order);

        bool TryToMove(int vx, int vy);
        
        void Follow(std::shared_ptr<Character> sprite);
        void Unfollow();
        std::vector<Vector2> GetActionQueries();

        bool IsNPC();

        std::string Serialize(std::string output);

    protected:
        int speed;
        int vx;
        int vy;
        std::string facing;
        bool npc;
        std::string spriteType;
        int handledInput;
        int oldX;
        int oldY;
        bool bootstrap;
        std::shared_ptr<Character> following;
        std::vector<Vector2> followBuffer;
        std::shared_ptr<Character> follower;
        Vector2 lastLeaderPos;

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}
#endif
