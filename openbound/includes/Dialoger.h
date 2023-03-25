#ifndef SBURB_DIALOGER_H
#define SBURB_DIALOGER_H

#include "Common.h"
#include <SFML/Graphics/Drawable.hpp>
#include "FontEngine.h"
#include "Sprite.h"

namespace SBURB
{
    class Dialoger : public sf::Drawable, public sf::Transformable
    {
    public:
        Dialoger(Vector2 hiddenPos, Vector2 alertPos, Vector2 talkPosLeft, Vector2 talkPosRight, Vector2 spriteStartRight, Vector2 spriteEndRight, Vector2 spriteStartLeft, Vector2 spriteEndLeft, Vector4 alertTextDimensions, Vector4 leftTextDimensions, Vector4 rightTextDimensions, std::wstring type);
        ~Dialoger();

        void HandleType();
        void Nudge();
        void SkipAll();

        void StartDialog(std::wstring info);
        void NextDialog();

        std::wstring OppositeSide(std::wstring side);
        std::shared_ptr<Sprite> DialogOnSide(std::wstring side);
        Vector2 StartOnSide(std::wstring side);
        Vector2 EndOnSide(std::wstring side);

        bool MoveToward(Vector2& startPos, Vector2 endPos, int speed = 100);
        bool MoveToward(std::shared_ptr<Sprite> sprite, Vector2 pos, int speed = 100);
        void Update();
        Vector4 DecideDialogDimensions();

        void SetBox(std::wstring box);
        std::wstring Serialize(std::wstring output);

        void SetQueue(std::vector<std::wstring> queue) { this->queue = queue; };
        std::vector<std::wstring> GetQueue() { return this->queue; };
        
        std::wstring GetQueueItem(int index) { return this->queue[index]; };

        void SetDialogSpriteLeft(std::shared_ptr<Sprite> dialogSpriteLeft) { this->dialogSpriteLeft = dialogSpriteLeft;  };
        std::shared_ptr<Sprite> GetDialogSpriteLeft() { return this->dialogSpriteLeft; };

        void SetDialogSpriteRight(std::shared_ptr<Sprite> dialogSpriteRight) { this->dialogSpriteRight = dialogSpriteRight; };
        std::shared_ptr<Sprite> GetDialogSpriteRight() { return this->dialogSpriteRight; };

        bool GetTalking() { return this->talking; };

        std::shared_ptr<Sprite> GetBox() { return this->box; };

    protected:
        std::wstring name;
        std::wstring currentDialog;
        std::wstring extraArgs;
        std::vector<std::wstring> queue;

        std::shared_ptr<FontEngine> dialog;
        std::shared_ptr<FontEngine> hashes;
        std::shared_ptr<Sprite> box;
        std::shared_ptr<Sprite> defaultBox;
        std::shared_ptr<Sprite> dialogSpriteLeft;
        std::shared_ptr<Sprite> dialogSpriteRight;
        std::shared_ptr<Sprite> graphic;

        Vector2 hiddenPos;
        Vector2 alertPos;
        Vector2 talkPosLeft;
        Vector2 talkPosRight;
        Vector2 spriteStartRight;
        Vector2 spriteEndRight;
        Vector2 spriteStartLeft;
        Vector2 spriteEndLeft;
        Vector4 alertTextDimensions;
        Vector4 leftTextDimensions;
        Vector4 rightTextDimensions;
        Vector2 pos;

        std::wstring actor;
        std::wstring dialogSide;
        std::wstring type;

        bool talking;
        bool inPosition;
        std::map<std::wstring, int> choices;

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}
#endif
