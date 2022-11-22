#include "Parser.h"
#include "AssetManager.h"
#include "Game.h"

namespace SBURB {
	std::string GetActionNodeText(pugi::xml_node node) {
		if (!node) return "";

		AssetManager* assetManager = &Game::GetInstance()->assetManager;

		for (pugi::xml_node child : node.children()) {
			if (child.name() == "args") {

				std::string asset = child.attribute("body").as_string();
				if (asset != "" && assetManager->CheckIsLoaded(asset)) {
					return assetManager->GetAsset(asset);
				}

				for (pugi::xml_node subChild : child.children()) {
					if (subChild.first_child()) {
						serializer = new XMLSerializer();

						std::string output = "";
						for (pugi::xml_node subSubChild : child.children()) {
							output += serializer.serializeToString(subSubChild);
						}

						return output;
					}
				}

				if (!child.text().empty()) {
					return child.text().as_string();
				}

				return child.first_child().value();
			}
		}

		if (!node.text().empty()) {
			return node.text().as_string();
		}

		return node.first_child().value();
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

			std::string info = GetActionNodeText(*curNode);
			info = trim(unescape(info));
			
			std::shared_ptr<Action> newAction = std::make_shared<Action>(
				curNode->attribute("command").as_string(),
				info,
				unescape(curNode->attribute("name").as_string()),
				targSprite,
				NULL,
				curNode->attribute("noWait").as_bool(),
				curNode->attribute("noDelay").as_bool(),
				times,
				curNode->attribute("soft").as_bool(),
				curNode->attribute("silent").as_bool());

			if (oldAction) {
				oldAction->SetFollowUp(newAction);
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
		std::shared_ptr<Action> newAction = NULL;
		std::vector<std::string> newGroups;
		bool newNoWait = false;
		bool newPaused = false;
		std::shared_ptr<Trigger> newTrigger = NULL;

		for (pugi::xml_node child : node.children()) {
			if (child.name()  == "#text") {
				continue;
			}
			if (child.name()  == "action") {
				newAction = std::make_shared<Action>(ParseAction(child));
			}
			else {
				newTrigger = std::make_shared<Trigger>(ParseTrigger(child));
			}
		}

		std::string newId = node.attribute("id").as_string();

		std::string tmpGroups = node.attribute("groups").as_string();
		if(tmpGroups != "") split(tmpGroups, ":");

		bool tmpNoWait = node.attribute("noWait").as_bool();
		if (tmpNoWait) newNoWait = tmpNoWait;

		bool tmpPaused = node.attribute("paused").as_bool();
		if (tmpPaused) newPaused = tmpPaused;

		return ActionQueue(newAction, newId, newGroups, newNoWait, newPaused, newTrigger);
    }

    Animation Parser::ParseAnimation(pugi::xml_node node) {
		// TODO: Sheet seems to be either a string or an Asset?? How to solve for this?
		std::string sheet = "";

		int colSize = 0;
		int rowSize = 0;

		bool sliced = node.attribute("sliced").as_bool();

		std::string name = node.attribute("name").as_string("image");

		std::string tmpSheet = node.attribute("sheet").as_string();
		if (!sliced) {
			sheet = assets[tmpSheet];
		}
		else {
			sheet = tmpSheet;
		}

		int x = node.attribute("x").as_int();
		int y = node.attribute("y").as_int();
		int length = node.attribute("length").as_int(1);

		int numCols = node.attribute("numCols").as_int(1);
		int numRows = node.attribute("numRows").as_int(1);

		int tmpColSize = node.attribute("colSize").as_int();
		if (tmpColSize) colSize = tmpColSize;
		else colSize = round(sheet.width / length);

		int tmpRowSize = node.attribute("rowSize").as_int();
		if (tmpRowSize) rowSize = tmpRowSize;
		else rowSize = sheet.height;

		int startPos = node.attribute("startPos").as_int();

		int frameInterval = node.attribute("frameInterval").as_int(1);
		int loopNum = node.attribute("loopNum").as_int(-1);
		std::string followUp = node.attribute("followUp").as_string();

		bool flipX = node.attribute("flipX").as_bool();
		bool flipY = node.attribute("flipY").as_bool();

		return Animation(name, sheet, x, y, colSize, rowSize, startPos, length, frameInterval, loopNum, followUp, flipX, flipY, sliced, numCols, numRows);
    }

    Character Parser::ParseCharacter(pugi::xml_node node) {
		Character newChar = Character(node.attribute("name").as_string(),
			node.attribute("x").as_int(),
			node.attribute("y").as_int(),
			node.attribute("width").as_int(),
			node.attribute("height").as_int(),
			node.attribute("sx").as_int(),
			node.attribute("sy").as_int(),
			node.attribute("sWidth").as_int(),
			node.attribute("sHeight").as_int(),
			assets[node.attribute("sheet").as_string()]);

		std::string tmpFollowing = node.attribute("following").as_string();
		if (tmpFollowing != "") {
			std::shared_ptr<Character> following = sprites[tmpFollowing];
			if (following) {
				newChar.Follow(following);
			}
		}

		std::string tmpFollower = node.attribute("follower").as_string();
		if (tmpFollower != "") {
			std::shared_ptr<Character> follower = sprites[tmpFollower];
			if (follower) {
				follower->Follow(&newChar);
			}
		}

		auto anims = node.children("animation");
		for (auto anim : anims) {
			Animation newAnim = ParseAnimation(anim);
			newChar.AddAnimation(newAnim);
		}
		newChar.StartAnimation(node.attribute("state").as_string());
		newChar.facing = node.attribute("facing").as_string();

		return newChar;
    }

	Vector2 parse2Dimensions(std::string in) {
		std::vector<std::string> values = split(in, ",");
		Vector2 dimensions = Vector2();

		dimensions.x = stoi(values[0]);

		return dimensions;
	}

	Vector4 parse4Dimensions(std::string in) {
		std::vector<std::string> values = split(in, ",");
		Vector4 dimensions = Vector4();

		dimensions.x = stoi(values[0]);
		dimensions.y = stoi(values[1]);
		dimensions.z = stoi(values[2]);
		dimensions.w = stoi(values[3]);

		return dimensions;
	}

    Dialoger Parser::ParseDialoger(pugi::xml_node node) {
		Vector2 hiddenPos = parse2Dimensions(node.attribute("hiddenPos").as_string());
		Vector2 alertPos = parse2Dimensions(node.attribute("alertPos").as_string());
		Vector2 talkPosLeft = parse2Dimensions(node.attribute("talkPosLeft").as_string());
		Vector2 talkPosRight = parse2Dimensions(node.attribute("talkPosRight").as_string());
		Vector2 spriteStartRight = parse2Dimensions(node.attribute("spriteStartRight").as_string());
		Vector2 spriteEndRight = parse2Dimensions(node.attribute("spriteEndRight").as_string());
		Vector2 spriteStartLeft = parse2Dimensions(node.attribute("spriteStartLeft").as_string());
		Vector2 spriteEndLeft = parse2Dimensions(node.attribute("spriteEndLeft").as_string());
		Vector4 alertTextDimensions = parse4Dimensions(node.attribute("alertTextDimensions").as_string());
		Vector4 leftTextDimensions = parse4Dimensions(node.attribute("leftTextDimensions").as_string());
		Vector4 rightTextDimensions = parse4Dimensions(node.attribute("rightTextDimensions").as_string());
		std::string type = node.attribute("type").as_string("standard");

		Dialoger newDialoger = Dialoger(hiddenPos, alertPos, talkPosLeft, talkPosRight,
			spriteStartRight, spriteEndRight, spriteStartLeft, spriteEndLeft,
			alertTextDimensions, leftTextDimensions, rightTextDimensions, type);

		std::string box = node.attribute("box").as_string();
		newDialoger.SetBox(box);

		return newDialoger;
    }

    Fighter Parser::ParseFighter(pugi::xml_node node) {
		std::string name = node.attribute("name").as_string();
		int x = node.attribute("x").as_int();
		int y = node.attribute("y").as_int();
		int width = node.attribute("width").as_int();
		int height = node.attribute("height").as_int();

		std::string newState = node.attribute("state").as_string();
		std::string newFacing = node.attribute("facing").as_string("Right");

		Fighter newSprite = Fighter(name, x, y, width, height);
		newSprite.SetFacing(newFacing);

		auto anims = node.children("animation");
		for (pugi::xml_node anim : anims) {
			Animation newAnim = ParseAnimation(anim);
			newSprite.AddAnimation(newAnim);

			if (newState == "") {
				newState = newAnim.GetName();
			}
		}

		newSprite.StartAnimation(newState);

		return newSprite;
    }

    Room Parser::ParseRoom(pugi::xml_node node) {
		Room newRoom = Room(node.attribute("name").as_string(),
			node.attribute("width").as_int(),
			node.attribute("height").as_int());

		int mapScale = node.attribute("mapScale").as_int();
		if (mapScale != 0) {
			newRoom.SetMapScale(mapScale);
		}

		std::string walkableMap = node.attribute("walkableMap").as_string();
		if (walkableMap != "") {
			newRoom.SetWalkableMap(assets[walkableMap]);
			if (!newRoom.GetWidth()) {
				newRoom.SetWidth(newRoom.GetWalkableMap().GetWidth() * newRoom.GetMapScale());
			}

			if (!newRoom.GetHeight()) {
				newRoom.SetHEight(newRoom.GetWalkableMap().GetHeight() * newRoom.GetMapScale());
			}
		}

		SerialLoadRoomSprites(newRoom, node.children("sprite"));
		SerialLoadRoomSprites(newRoom, node.children("character"));
		SerialLoadRoomSprites(newRoom, node.children("fighter"));

		auto paths = node.children("paths");
		if (!paths.empty()) {
			SerialLoadRoomPaths(newRoom, paths);
		}

		auto triggers = node.children("triggers");
		if (!triggers.empty()) {
			SerialLoadRoomTriggers(newRoom, triggers);
		}

		return newRoom;
    }

    Sprite Parser::ParseSprite(pugi::xml_node node) {

    }

    SpriteButton Parser::ParseSpriteButton(pugi::xml_node node) {

    }

    Trigger Parser::ParseTrigger(pugi::xml_node node) {

    }
}