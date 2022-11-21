#ifndef SBURB_CHARACTER_H
#define SBURB_CHARACTER_H

#include "Common.h"

namespace SBURB
{
    class Character
    {
    public:
        Character(std::string name, int x, int y, int width, int height, int sx, int sy, int sWidth, int sHeight, std::string sheet, bool bootstrap);
        ~Character();

    protected:


    };
}
#endif
