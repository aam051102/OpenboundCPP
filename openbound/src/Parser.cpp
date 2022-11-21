#include "Parser.h"

namespace SBURB {
	std::string GetNodeText(pugi::xml_node node) {
		return "";
	}

    Action Parser::ParseAction(pugi::xml_node node) {
		pugi::xml_node* curNode = &node;
		std::string targSprite = "";
		std::shared_ptr<Action> firstAction = NULL;
		std::shared_ptr<Action> oldAction = NULL;

		do {
			if (curNode->attribute("sprite").as_string() != "null") {
				targSprite = curNode->attribute("sprite").as_string();
			}

			int timesAttr = curNode->attribute("times").as_int();
			int forAttr = curNode->attribute("for").as_int();
			int loopsAttr = curNode->attribute("loops").as_int();

			int times = 1;
			if(timesAttr) times = timesAttr;
			else if (loopsAttr) times = loopsAttr;
			else if (forAttr) times = forAttr;

			std::string info = GetNodeText(*curNode);
			info = trim(unescape(info));
			
			std::shared_ptr<Action> newAction = std::shared_ptr<Action>(new Action(
				curNode->attribute("command").as_string(),
				info,
				unescape(curNode->attribute("name").as_string()),
				targSprite,
				NULL,
				curNode->attribute("noWait").as_bool(),
				curNode->attribute("noDelay").as_bool(),
				times,
				curNode->attribute("soft").as_bool(),
				curNode->attribute("silent").as_bool()));

			if (oldAction) {
				oldAction->SetFollowUp(std::shared_ptr<Action>(newAction));
			}

			if (!firstAction) {
				firstAction = newAction;
			}

			oldAction = newAction;
			pugi::xml_node* oldNode = curNode;
			curNode = NULL;

			for (pugi::xml_node child : oldNode->children()) {
				if (child.name() == "action") {
					curNode = &child;
					break;
				}
			}
			if (!curNode) {
				break;
			}
		} while (curNode);

		return *firstAction;
    }

    ActionQueue Parser::ParseActionQueue(pugi::xml_node node) {

    }

    Animation Parser::ParseAnimation(pugi::xml_node node) {

    }

    Character Parser::ParseCharacter(pugi::xml_node node, std::string assetolder) {

    }

    Dialoger Parser::ParseDialoger(pugi::xml_node node) {

    }

    Fighter Parser::ParseFighter(pugi::xml_node node, std::string assetFolder) {

    }

    Room Parser::ParseRoom(pugi::xml_node node, std::string assetFolder, std::string spriteFolder) {

    }

    Sprite Parser::ParseSprite(pugi::xml_node node, std::string assetFolder) {

    }

    SpriteButton Parser::ParseSpriteButton(pugi::xml_node node) {

    }

    Trigger Parser::ParseTrigger(pugi::xml_node node) {

    }
}