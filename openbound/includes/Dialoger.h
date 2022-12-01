#ifndef SBURB_DIALOGER_H
#define SBURB_DIALOGER_H

#include "Common.h"
#include <SFML/Graphics/Drawable.hpp>
#include "FontEngine.h"
#include "Sprite.h"
#include "Sburb.h"

namespace SBURB
{
    class Dialoger : public sf::Drawable
    {
    public:
        Dialoger(Vector2 hiddenPos, Vector2 alertPos, Vector2 talkPosLeft, Vector2 talkPosRight, Vector2 spriteStartRight, Vector2 spriteEndRight, Vector2 spriteStartLeft, Vector2 spriteEndLeft, Vector4 alertTextDimensions, Vector4 leftTextDimensions, Vector4 rightTextDimensions, std::string type);
        ~Dialoger();

        void HandleType();
        void Nudge();
        void SkipAll();

        void StartDialog(std::string info);
        void NextDialog();

        std::string OppositeSide(std::string side);
        std::shared_ptr<Sprite> DialogOnSide(std::string side);
        Vector2 StartOnSide(std::string side);
        Vector2 EndOnSide(std::string side);

        bool MoveToward(Vector2 startPos, Vector2 endPos, int speed = 100);
        bool MoveToward(std::shared_ptr<Sprite> sprite, Vector2 pos, int speed = 100);
        void Update();
        Vector4 DecideDialogDimensions();

        void SetBox(std::string box);
        std::string Serialize(std::string output);

        void SetQueue(std::vector<std::string> queue) { this->queue = queue; };
        std::vector<std::string> GetQueue() { return this->queue; };
        
        std::string GetQueueItem(int index) { return this->queue[index]; };

        void SetDialogSpriteLeft(std::shared_ptr<Sprite> dialogSpriteLeft) { this->dialogSpriteLeft = dialogSpriteLeft;  };
        std::shared_ptr<Sprite> GetDialogSpriteLeft() { return this->dialogSpriteLeft; };

        void SetDialogSpriteRight(std::shared_ptr<Sprite> dialogSpriteRigght) { this->dialogSpriteRight = dialogSpriteRight; };
        std::shared_ptr<Sprite> GetDialogSpriteRight() { return this->dialogSpriteRight; };

        bool GetTalking() { return this->talking; };

    protected:
        std::string name;
        std::string currentDialog;
        std::string extraArgs;
        std::vector<std::string> queue;

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

        std::string actor;
        std::string dialogSide;
        std::string type;

        bool talking;
        bool inPosition;
        std::map<std::string, int> choices;

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}
#endif
