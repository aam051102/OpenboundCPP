#include "Parser.h"
#include "AssetManager.h"
#include "Sburb.h"
#include "Serializer.h"

namespace SBURB
{
	std::shared_ptr<Sprite> Parser::ParseCharacterString(std::string spriteName)
	{
		if (spriteName == "char")
		{
			return Sburb::GetInstance()->GetCharacter();
		}
		else
		{
			return Sburb::GetInstance()->GetSprite(spriteName);
		}
	}

	std::vector<std::shared_ptr<Action>> Parser::ParseActionString(std::string str)
	{
		std::vector<std::shared_ptr<Action>> actions = {};

		pugi::xml_document input = Serializer::ParseXML(str);

		for (pugi::xml_node tmp : input.children())
		{
			if (std::string(tmp.name()) == "action")
			{
				actions.push_back(Parser::ParseAction(tmp));
			}
		}

		return actions;
	}

	std::vector<std::shared_ptr<Trigger>> Parser::ParseTriggerString(std::string str)
	{
		std::vector<std::shared_ptr<Trigger>> triggers = {};

		pugi::xml_document input = Serializer::ParseXML(str);

		for (pugi::xml_node tmp : input.children())
		{
			if (std::string(tmp.name()) == "trigger")
			{
				triggers.push_back(Parser::ParseTrigger(tmp));
			}
		}

		return triggers;
	}

	std::string Parser::ParseURLString(std::string str)
	{
		if (str.find("/") == 0)
		{
			return str.substr(1);
		}
		else if (str.find("://") == std::string::npos)
		{
			return "http://" + str;
		}

		return str;
	}

	std::string GetActionNodeText(pugi::xml_node node)
	{
		if (!node)
			return "";

		std::ostringstream serializeStream;

		for (pugi::xml_node child : node.children())
		{
			if (std::string(child.name()) == "args")
			{
				std::string asset = child.attribute("body").as_string();
				if (asset != "" && AssetManager::GetTextByName(asset))
				{
					return AssetManager::GetTextByName(asset)->GetText();
				}

				for (pugi::xml_node subChild : child.children())
				{
					if (subChild.first_child())
					{
						std::string output = "";

						for (pugi::xml_node subSubChild : child.children())
						{
							serializeStream.str(std::string());
							subSubChild.print(serializeStream, "", pugi::format_raw);
							output += serializeStream.str();

						}
						
						return output;
					}
				}
				
				if (!child.text().empty())
				{
					return child.text().as_string();
				}

				return child.first_child().value();
			}
		}

		if (!node.text().empty())
		{
			return node.text().as_string();
		}

		return node.first_child().value();
	}

	std::shared_ptr<Action> Parser::ParseAction(pugi::xml_node node)
	{
		pugi::xml_node *curNode = &node;
		std::string targSprite = "";
		std::shared_ptr<Action> firstAction = NULL;
		std::shared_ptr<Action> oldAction = NULL;

		do
		{
			if (std::string(curNode->attribute("sprite").as_string()) != "null")
			{
				targSprite = curNode->attribute("sprite").as_string();
			}

			int timesAttr = curNode->attribute("times").as_int();
			int forAttr = curNode->attribute("for").as_int();
			int loopsAttr = curNode->attribute("loops").as_int();

			int times = 1;
			if (timesAttr)
				times = timesAttr;
			else if (loopsAttr)
				times = loopsAttr;
			else if (forAttr)
				times = forAttr;

			std::string info = GetActionNodeText(*curNode);
			info = trim(unescape(info));

			std::shared_ptr<Action> newAction = std::make_shared<Action>(
				std::string(curNode->attribute("command").as_string()),
				info,
				unescape(curNode->attribute("name").as_string()),
				targSprite,
				nullptr,
				curNode->attribute("noWait").as_bool(),
				curNode->attribute("noDelay").as_bool(),
				times,
				curNode->attribute("soft").as_bool(),
				curNode->attribute("silent").as_string());

			if (oldAction)
			{
				oldAction->SetFollowUp(newAction);
			}

			if (!firstAction)
			{
				firstAction = newAction;
			}

			oldAction = newAction;
			pugi::xml_node *oldNode = curNode;
			curNode = nullptr;

			for (pugi::xml_node child : oldNode->children())
			{
				if (std::string(child.name()) == "action")
				{
					curNode = &child;
					break;
				}
			}
			if (!curNode)
			{
				break;
			}
		} while (curNode);

		return firstAction;
	}

	std::shared_ptr<ActionQueue> Parser::ParseActionQueue(pugi::xml_node node)
	{
		std::shared_ptr<Action> newAction = NULL;
		std::vector<std::string> newGroups;
		bool newNoWait = false;
		bool newPaused = false;
		std::shared_ptr<Trigger> newTrigger = NULL;

		for (pugi::xml_node child : node.children())
		{
			if (std::string(child.name()) == "#text")
			{
				continue;
			}
			if (std::string(child.name()) == "action")
			{
				newAction = ParseAction(child);
			}
			else
			{
				newTrigger = ParseTrigger(child);
			}
		}

		std::string newId = node.attribute("id").as_string();

		std::string tmpGroups = node.attribute("groups").as_string();
		if (tmpGroups != "")
			split(tmpGroups, ":");

		bool tmpNoWait = node.attribute("noWait").as_bool();
		if (tmpNoWait)
			newNoWait = tmpNoWait;

		bool tmpPaused = node.attribute("paused").as_bool();
		if (tmpPaused)
			newPaused = tmpPaused;

		return std::make_shared<ActionQueue>(newAction, newId, newGroups, newNoWait, newPaused, newTrigger);
	}

	std::shared_ptr<Animation> Parser::ParseAnimation(pugi::xml_node node)
	{
		int colSize = 0;
		int rowSize = 0;

		bool sliced = node.attribute("sliced").as_bool();

		std::string name = node.attribute("name").as_string("image");

		std::shared_ptr<AssetGraphic> sheet;
		std::string tmpSheet = node.attribute("sheet").as_string();

		if (!sliced)
		{
			sheet = AssetManager::GetGraphicByName(tmpSheet);
		}

		if(sheet) sheet->Load();

		int x = node.attribute("x").as_int();
		int y = node.attribute("y").as_int();
		int length = node.attribute("length").as_int(1);

		int numCols = node.attribute("numCols").as_int(1);
		int numRows = node.attribute("numRows").as_int(1);

		int tmpColSize = node.attribute("colSize").as_int();
		if (tmpColSize)
			colSize = tmpColSize;
		else if (sheet)
			colSize = round(sheet->GetAsset()->getSize().x / length);

		int tmpRowSize = node.attribute("rowSize").as_int();
		if (tmpRowSize)
			rowSize = tmpRowSize;
		else if (sheet)
			rowSize = sheet->GetAsset()->getSize().y;

		if(sheet) sheet->Unload();

		int startPos = node.attribute("startPos").as_int();

		int frameInterval = node.attribute("frameInterval").as_int(1);
		int loopNum = node.attribute("loopNum").as_int(-1);
		std::string followUp = node.attribute("followUp").as_string();

		bool flipX = node.attribute("flipX").as_bool();
		bool flipY = node.attribute("flipY").as_bool();

		return std::make_shared<Animation>(name, tmpSheet, x, y, colSize, rowSize, startPos, length, std::to_string(frameInterval), loopNum, followUp, flipX, flipY, sliced, numCols, numRows);
	}

	std::shared_ptr<Character> Parser::ParseCharacter(pugi::xml_node node)
	{
		auto newChar = std::make_shared<Character>(std::string(node.attribute("name").as_string()),
									  node.attribute("x").as_int(),
									  node.attribute("y").as_int(),
									  node.attribute("width").as_int(),
									  node.attribute("height").as_int(),
									  node.attribute("sx").as_int(),
									  node.attribute("sy").as_int(),
									  node.attribute("sWidth").as_int(),
									  node.attribute("sHeight").as_int(),
									  node.attribute("sheet").as_string());

		std::string tmpFollowing = node.attribute("following").as_string();

		if (tmpFollowing != "")
		{
			std::shared_ptr<Character> following = std::static_pointer_cast<Character>(Sburb::GetInstance()->GetSprite(tmpFollowing));
			if (following)
			{
				newChar->Follow(following);
			}
		}

		std::string tmpFollower = node.attribute("follower").as_string();
		if (tmpFollower != "")
		{
			std::shared_ptr<Character> follower = std::static_pointer_cast<Character>(Sburb::GetInstance()->GetSprite(tmpFollower));
			if (follower)
			{
				follower->Follow(newChar);
			}
		}

		auto anims = GetNestedChildren(&node, "animation");

		for (auto anim : anims)
		{
			std::shared_ptr<Animation> newAnim = ParseAnimation(anim);
			newChar->AddAnimation(newAnim);
		}
		newChar->StartAnimation(node.attribute("state").as_string());
		newChar->SetFacing(node.attribute("facing").as_string());

		return newChar;
	}

	Vector2 parse2Dimensions(std::string in)
	{
		std::vector<std::string> values = split(in, ",");
		Vector2 dimensions = Vector2();

		dimensions.x = stoi(values[0]);
		dimensions.y = stoi(values[1]);

		return dimensions;
	}

	Vector4 parse4Dimensions(std::string in)
	{
		std::vector<std::string> values = split(in, ",");
		Vector4 dimensions = Vector4();

		dimensions.x = stoi(values[0]);
		dimensions.y = stoi(values[1]);
		dimensions.z = stoi(values[2]);
		dimensions.w = stoi(values[3]);

		return dimensions;
	}

	std::shared_ptr<Dialoger> Parser::ParseDialoger(pugi::xml_node node)
	{
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

		auto newDialoger = std::make_shared<Dialoger>(hiddenPos, alertPos, talkPosLeft, talkPosRight,
										spriteStartRight, spriteEndRight, spriteStartLeft, spriteEndLeft,
										alertTextDimensions, leftTextDimensions, rightTextDimensions, type);

		std::string box = node.attribute("box").as_string();
		newDialoger->SetBox(box);

		return newDialoger;
	}

	std::shared_ptr<Fighter> Parser::ParseFighter(pugi::xml_node node)
	{
		std::string name = node.attribute("name").as_string();
		int x = node.attribute("x").as_int();
		int y = node.attribute("y").as_int();
		int width = node.attribute("width").as_int();
		int height = node.attribute("height").as_int();

		std::string newState = node.attribute("state").as_string();
		std::string newFacing = node.attribute("facing").as_string("Right");

		auto newSprite = std::make_shared<Fighter>(name, x, y, width, height);
		newSprite->SetFacing(newFacing);

		auto anims = GetNestedChildren(&node, "animation");
		for (pugi::xml_node anim : anims)
		{
			std::shared_ptr<Animation> newAnim = ParseAnimation(anim);
			newSprite->AddAnimation(newAnim);

			if (newState == "")
			{
				newState = newAnim->GetName();
			}
		}

		newSprite->StartAnimation(newState);

		return newSprite;
	}

	std::shared_ptr<Room> Parser::ParseRoom(pugi::xml_node node)
	{
		auto newRoom = std::make_shared<Room>(node.attribute("name").as_string(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int());

		int mapScale = node.attribute("mapScale").as_int();
		if (mapScale != 0)
		{
			newRoom->SetMapScale(mapScale);
		}

		std::string walkableMap = node.attribute("walkableMap").as_string();
		if (walkableMap != "")
		{
			newRoom->SetWalkableMap(AssetManager::GetGraphicByName(walkableMap));
			if (!newRoom->GetWidth())
			{
				newRoom->SetWidth(newRoom->GetWalkableMap()->GetAsset()->getSize().x * newRoom->GetMapScale());
			}

			if (!newRoom->GetHeight())
			{
				newRoom->SetHeight(newRoom->GetWalkableMap()->GetAsset()->getSize().y * newRoom->GetMapScale());
			}
		}

		Serializer::SerialLoadRoomSprites(newRoom, GetNestedChildren(&node, "sprite"));
		Serializer::SerialLoadRoomSprites(newRoom, GetNestedChildren(&node, "character"));
		Serializer::SerialLoadRoomSprites(newRoom, GetNestedChildren(&node, "fighter"));

		auto paths = node.child("paths");
		if (paths)
		{
			Serializer::SerialLoadRoomPaths(newRoom, paths);
		}

		auto triggers = node.child("triggers");
		if (triggers)
		{
			Serializer::SerialLoadRoomTriggers(newRoom, triggers);
		}

		return newRoom;
	}

	std::shared_ptr<Sprite> Parser::ParseSprite(pugi::xml_node node)
	{
		std::string name = node.attribute("name").as_string();
		int x = node.attribute("x").as_int();
		int y = node.attribute("y").as_int();
		int width = node.attribute("width").as_int();
		int height = node.attribute("height").as_int();
		int dx = node.attribute("dx").as_int();
		int dy = node.attribute("dy").as_int();
		int depthing = node.attribute("depthing").as_int();
		bool collidable = node.attribute("collidable").as_bool();
		std::string state = node.attribute("state").as_string();

		auto newSprite = std::make_shared<Sprite>(name, x, y, width, height, dx, dy, depthing, collidable);

		auto anims = GetNestedChildren(&node, "animation");

		for (pugi::xml_node anim : anims)
		{
			std::shared_ptr<Animation> newAnim = ParseAnimation(anim);
			newSprite->AddAnimation(newAnim);

			if (state == "")
			{
				state = newAnim->GetName();
			}
		}

		if (anims.empty())
		{
			std::shared_ptr<AssetGraphic> asset = AssetManager::GetGraphicByName(name);

			if (asset && asset->GetType() == "graphic")
			{
				newSprite->AddAnimation(std::make_shared<Animation>(std::string("image"), name));
				state = "image";
			}
		}

		newSprite->StartAnimation(state);

		return newSprite;
	}

	std::shared_ptr<SpriteButton> Parser::ParseSpriteButton(pugi::xml_node node)
	{
		std::shared_ptr<AssetGraphic> sheet = AssetManager::GetGraphicByName(node.attribute("sheet").as_string());
		sheet->Load();

		auto newButton = std::make_shared<SpriteButton>(node.attribute("name").as_string(),
											  node.attribute("x").as_int(),
											  node.attribute("y").as_int(),
											  node.attribute("width").as_int(sheet->GetAsset()->getSize().x),
											  node.attribute("height").as_int(sheet->GetAsset()->getSize().y),
											  node.attribute("sheet").as_string(),
											  nullptr);
		sheet->Unload();

		auto action = node.child("action");
		if (action)
		{
			std::shared_ptr<Action> newAction = ParseAction(action);
			newButton->SetAction(newAction);
		}

		return newButton;
	}

	std::vector<std::string> GetTriggerNodeText(pugi::xml_node node)
	{
		std::vector<std::string> outputs = {};

		std::ostringstream serializeStream;

		for (pugi::xml_node child : node.children())
		{
			if (std::string(child.name()) == "args")
			{
				for (pugi::xml_node subChild : child.children())
				{
					if (subChild.first_child())
					{
						std::string output = "";
						for (pugi::xml_node subSubChild : child.children())
						{
							serializeStream.str(std::string());
							subSubChild.print(serializeStream, "", pugi::format_raw);
							output += serializeStream.str();
						}

						outputs.push_back(output);
					}
				}

				if (!child.text().empty())
				{
					outputs.push_back(child.text().as_string());
				}
				else
				{
					outputs.push_back(child.first_child().value());
				}
			}
		}

		if (outputs.size() == 0)
		{
			outputs.push_back(node.first_child().value());
		}

		return outputs;
	}

	std::shared_ptr<Trigger> Parser::ParseTrigger(pugi::xml_node node)
	{
		pugi::xml_node *curNode = &node;
		std::shared_ptr<Trigger> firstTrigger = nullptr;
		std::shared_ptr<Trigger> oldTrigger = nullptr;

		do
		{
			std::vector<std::string> info = GetTriggerNodeText(*curNode);
			for (int i = 0; i < info.size(); i++)
			{
				info[i] = unescape(info[i]);
			}

			auto action = GetNestedChild(curNode, "action");

			std::shared_ptr<Action> curAction = nullptr;

			if (action && action.parent() == *curNode)
			{
				curAction = ParseAction(action);
			}

			bool restart = curNode->attribute("restart").as_bool();
			bool detonate = curNode->attribute("detonate").as_bool();
			std::string op = curNode->attribute("operator").as_string();

			std::shared_ptr<Trigger> trigger = std::make_shared<Trigger>(info, curAction, nullptr, restart, detonate, op);

			if (!firstTrigger)
			{
				firstTrigger = trigger;
			}
			if (oldTrigger)
			{
				oldTrigger->SetFollowUp(trigger);
			}
			oldTrigger = trigger;

			auto child = GetNestedChild(curNode, "trigger");

			if (child)
			{
				curNode = &child;
			}
			else {
				break;
			}
		} while (curNode);

		return firstTrigger;
	}
}