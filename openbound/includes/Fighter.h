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
        Fighter(std::wstring name, int x, int y, int width, int height);
        ~Fighter();
        
        void Update();

        void HandleInputs(std::map<sf::Keyboard::Key, bool> pressed);
        
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
        
        std::vector<int> GetActionQueries();
        
        bool Collides(std::shared_ptr<Sprite> sprite, int dx = 0, int dy = 0);
        
        std::map<std::wstring, sf::Vector2f> GetBoundaryQueries(int dx = 0, int dy = 0);
        
        bool TryToMove();

        std::wstring Serialize(std::wstring output);

        void SetFacing(std::wstring facing) { this->facing = facing; };

    protected:
        float accel;
        float decel;
        float friction;
        float vx;
        float vy;
        std::wstring facing;

    };
}
#endif
