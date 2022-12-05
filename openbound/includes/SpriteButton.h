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
        SpriteButton(std::string name, int x, int y, int width, int height, std::string sheetName, std::shared_ptr<Action> action);
        ~SpriteButton();

        void Update() override;
        void UpdateMouse();

        void SetState(std::string state);

        void SetAction(std::shared_ptr<Action> action) { this->action = action; };

        std::string Serialize(std::string output);

    protected:
        std::shared_ptr<AssetGraphic> sheet;
        bool pressed = false;
        bool mousePressed = false;
        bool clicked = false;
        std::shared_ptr<Action> action;
        std::string sheetName;

    };
}
#endif
