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
        static std::shared_ptr<Sprite> ParseCharacterString(std::wstring spriteName);
        static std::vector<std::shared_ptr<Action>> ParseActionString(std::wstring str);
        static std::vector<std::shared_ptr<Trigger>> ParseTriggerString(std::wstring str);
        static std::wstring ParseURLString(std::wstring str);


        static std::shared_ptr<Action> ParseAction(pugi::xml_node node);
        static std::shared_ptr<ActionQueue>ParseActionQueue(pugi::xml_node node);
        static std::shared_ptr<Animation>ParseAnimation(pugi::xml_node node);
        static std::shared_ptr<Character>ParseCharacter(pugi::xml_node node);
        static std::shared_ptr<Dialoger>ParseDialoger(pugi::xml_node node);
        static std::shared_ptr<Fighter>ParseFighter(pugi::xml_node node);
        static std::shared_ptr<Room>ParseRoom(pugi::xml_node node);
        static std::shared_ptr<Sprite>ParseSprite(pugi::xml_node node);
        static std::shared_ptr<SpriteButton>ParseSpriteButton(pugi::xml_node node);
        static std::shared_ptr<Trigger>ParseTrigger(pugi::xml_node node);

    };
}
#endif
