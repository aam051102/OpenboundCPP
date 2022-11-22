#ifndef SBURB_PARSER_H
#define SBURB_PARSER_H

#include <pugixml.hpp>
#include "Common.h"
#include "Event.h"
#include "Room.h"
#include "Action.h"
#include "ActionQueue.h"
#include "Animation.h"
#include "Character.h"
#include "Dialoger.h"
#include "Fighter.h"
#include "SpriteButton.h"
#include "Trigger.h"

namespace SBURB
{
    class Parser
    {
    public:
        static Action ParseAction(pugi::xml_node node);
        static ActionQueue ParseActionQueue(pugi::xml_node node);
        static Animation ParseAnimation(pugi::xml_node node);
        static Character ParseCharacter(pugi::xml_node node);
        static Dialoger ParseDialoger(pugi::xml_node node);
        static Fighter ParseFighter(pugi::xml_node node);
        static Room ParseRoom(pugi::xml_node node);
        static Sprite ParseSprite(pugi::xml_node node);
        static SpriteButton ParseSpriteButton(pugi::xml_node node);
        static Trigger ParseTrigger(pugi::xml_node node);

    };
}
#endif
