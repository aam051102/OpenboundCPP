#ifndef SBURB_DIALOGER_H
#define SBURB_DIALOGER_H

#include "Common.h"

namespace SBURB
{
    class Dialoger
    {
    public:
        Dialoger(Vector2 hiddenPos, Vector2 alertPos, Vector2 talkPosLeft, Vector2 talkPosRight, Vector2 spriteStartRight, Vector2 spriteEndRight, Vector2 spriteStartLeft, Vector2 spriteEndLeft, Vector4 alertTextDimensions, Vector4 leftTextDimensions, Vector4 rightTextDimensions, std::string type);
        ~Dialoger();

        void SetBox(std::string box) { this->box = box; };

    protected:
        std::string box;

    };
}
#endif
