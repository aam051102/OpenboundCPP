#ifndef SBURB_SPRITE_BUTTON_H
#define SBURB_SPRITE_BUTTON_H

#include "Common.h"

namespace SBURB
{
    class SpriteButton
    {
    public:
        SpriteButton(std::string name, int x, int y, int width, int height, std::string sheet, std::shared_ptr<Action> action);
        ~SpriteButton();

    protected:


    };
}
#endif
