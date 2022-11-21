#ifndef SBURB_CHARACTER_H
#define SBURB_CHARACTER_H

#include "Common.h"

namespace SBURB
{
    class Character : public Sprite
    {
    public:
        Character(std::string name, int x, int y, int width, int height, int sx, int sy, int sWidth, int sHeight, std::string sheet, bool bootstrap = false);
        ~Character();

        std::string Serialize(std::string output);
        void Update();
        void HandleFollowing(Room room);

        void MoveUp(bool movingSideways);
        void MoveDown(bool movingSideways);
        void MoveLeft();
        void MoveRight();
        void MoveNone();
        void Walk();
        void Idle();

        void BecomeNPC();
        void BecomePlayer();

        void HandleInputs();

        void TryToMove(int vx, int vy, Room room);
        
        void Follow(Sprite* sprite);
        void Unfollow();
        void GetActionQueries();

        bool IsNPC();

    protected:


    };
}
#endif
