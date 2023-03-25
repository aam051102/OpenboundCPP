#ifndef SBURB_SPRITE_BUTTON_H
#define SBURB_SPRITE_BUTTON_H

#include "Common.h"
#include "Action.h"
#include "Sprite.h"
#include "AssetGraphic.h"

namespace SBURB
{
    class SpriteButton : public Sprite
    {
    public:
        SpriteButton(std::wstring name, int x, int y, int width, int height, std::wstring sheetName, std::shared_ptr<Action> action);
        ~SpriteButton();

        void Update() override;
        void UpdateMouse();

        void SetState(std::wstring state);

        void SetAction(std::shared_ptr<Action> action) { this->action = action; };

        std::wstring Serialize(std::wstring output);

    protected:
        std::shared_ptr<AssetGraphic> sheet;
        bool pressed = false;
        bool mousePressed = false;
        bool clicked = false;
        std::shared_ptr<Action> action;
        std::wstring sheetName;

    };
}
#endif
