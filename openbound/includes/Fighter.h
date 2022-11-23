#ifndef SBURB_FIGHTER_H
#define SBURB_FIGHTER_H

#include "Common.h"

namespace SBURB
{
    class Fighter : public Sprite
    {
    public:
        Fighter(std::string name, int x, int y, int width, int height);
        ~Fighter();

        void SetFacing(std::string facing) { this->facing = facing; };

    protected:
        std::string facing;

    };
}
#endif
