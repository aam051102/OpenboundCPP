#ifndef SBURB_SPRITE_BUTTON_H
#define SBURB_SPRITE_BUTTON_H

#include "Common.h"
#include "Action.h"
#include "Sprite.h"

namespace SBURB
{
    class SpriteButton : public Sprite
    {
    public:
        SpriteButton(std::string name, int x, int y, int width, int height, std::string sheetName, std::shared_ptr<Action> action);
        ~SpriteButton();

        void Update();
        void UpdateMouse();

        void SetState(std::string state);

        std::string Serialize(std::string output);

    protected:
        std::shared_ptr<AssetTexture> sheet;
        bool pressed = false;
        bool mousePressed = false;
        bool clicked = false;
        std::shared_ptr<Action> action;

    };
}
#endif
