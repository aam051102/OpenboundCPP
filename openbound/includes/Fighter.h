#ifndef SBURB_FIGHTER_H
#define SBURB_FIGHTER_H

#include "Common.h"
#include "Sprite.h"
#include "Input.h"

namespace SBURB
{
    class Fighter : public Sprite
    {
    public:
        Fighter(std::string name, int x, int y, int width, int height);
        ~Fighter();
        
        void Update();

        void HandleInputs(std::map<InputActions, bool> pressed);
        
        void Idle();
        void Walk();
        void Attack();
        void MoveUp();
        void MoveDown();
        void MoveLeft();
        void MoveRight();
        void MoveNone();
        
        void BecomePlayer();
        void BecomeNPC();
        
        void GetActionQueries();
        
        bool Collides(std::shared_ptr<Sprite> sprite, int dx, int dy);
        
        std::map<std::string, Vector2> GetBoundaryQueries(int dx, int dy);
        
        bool TryToMove();

        std::string Serialize(std::string output);

        void SetFacing(std::string facing) { this->facing = facing; };

    protected:
        float accel;
        float decel;
        float friction;
        int vx;
        int vy;
        std::string facing;

    };
}
#endif
