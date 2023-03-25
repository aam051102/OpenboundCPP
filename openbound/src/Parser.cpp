#include "Parser.h"
#include "AssetManager.h"
#include "Sburb.h"
#include "Serializer.h"

namespace SBURB
{
	std::shared_ptr<Sprite> Parser::ParseCharacterString(std::wstring spriteName)
	{
		if (spriteName == L"char")
		{
			return Sburb::GetInstance()->GetCharacter();
		}
		else
		{
			return Sburb::GetInstance()->GetSprite(spriteName);
		}
	}

	std::vector<std::shared_ptr<Action>> Parser::ParseActionString(std::wstring str)
	{
		std::vector<std::shared_ptr<Action>> actions = {};

		pugi::xml_document input = Serializer::ParseXML(str);

		for (pugi::xml_node tmp : input.children())
		{
			if (std::wstring(tmp.name()) == L"action")
			{
				actions.push_back(Parser::ParseAction(tmp));
			}
		}

		return actions;
	}

	std::vector<std::shared_ptr<Trigger>> Parser::ParseTriggerString(std::wstring str)
	{
		std::vector<std::shared_ptr<Trigger>> triggers = {};

		pugi::xml_document input = Serializer::ParseXML(str);

		for (pugi::xml_node tmp : input.children())
		{
			if (std::wstring(tmp.name()) == L"trigger")
			{
				triggers.push_back(Parser::ParseTrigger(tmp));
			}
		}

		return triggers;
	}

	std::wstring Parser::ParseURLString(std::wstring str)
	{
		if (str.find(L"/") == 0)
		{
			return str.substr(1);
		}
		else if (str.find(L"://") == std::wstring::npos)
		{
			return L"http://" + str;
		}

		return str;
	}

	std::wstring GetActionNodeText(pugi::xml_node node)
	{
		if (!node)
			return L"";

		std::wostringstream serializeStream;

		for (pugi::xml_node child : node.children())
		{
			if (std::wstring(child.name()) == L"args")
			{
				std::wstring asset = child.attribute(L"body").as_string();
				if (asset != L"" && AssetManager::GetTextByName(asset))
				{
					return AssetManager::GetTextByName(asset)->GetText();
				}

				for (pugi::xml_node subChild : child.children())
				{
					if (subChild.first_child())
					{
						std::wstring output = L"";

						for (pugi::xml_node subSubChild : child.children())
						{
							serializeStream.str(std::wstring());
							subSubChild.print(serializeStream, L"", pugi::format_raw);
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
		std::wstring targSprite;
		std::shared_ptr<Action> firstAction = NULL;
		std::shared_ptr<Action> oldAction = NULL;

		do
		{
			if (std::wstring(curNode->attribute(L"sprite").as_string()) != L"null")
			{
				targSprite = curNode->attribute(L"sprite").as_string();
			}

			int timesAttr = curNode->attribute(L"times").as_int();
			int forAttr = curNode->attribute(L"for").as_int();
			int loopsAttr = curNode->attribute(L"loops").as_int();

			int times = 1;
			if (timesAttr)
				times = timesAttr;
			else if (loopsAttr)
				times = loopsAttr;
			else if (forAttr)
				times = forAttr;

			std::wstring info = GetActionNodeText(*curNode);
			info = trim(unescape(info));

			std::shared_ptr<Action> newAction = std::make_shared<Action>(
				curNode->attribute(L"command").as_string(),
				info,
				unescape(curNode->attribute(L"name").as_string()),
				targSprite,
				nullptr,
				curNode->attribute(L"noWait").as_bool(),
				curNode->attribute(L"noDelay").as_bool(),
				times,
				curNode->attribute(L"soft").as_bool(),
				curNode->attribute(L"silent").as_string());

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
				if (std::wstring(child.name()) == L"action")
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
		std::vector<std::wstring> newGroups;
		bool newNoWait = false;
		bool newPaused = false;
		std::shared_ptr<Trigger> newTrigger = NULL;

		for (pugi::xml_node child : node.children())
		{
			if (std::wstring(child.name()) == L"#text")
			{
				continue;
			}
			if (std::wstring(child.name()) == L"action")
			{
				newAction = ParseAction(child);
			}
			else
			{
				newTrigger = ParseTrigger(child);
			}
		}

		std::wstring newId = node.attribute(L"id").as_string();

		std::wstring tmpGroups = node.attribute(L"groups").as_string();
		if (tmpGroups != L"")
			split(tmpGroups, L":");

		bool tmpNoWait = node.attribute(L"noWait").as_bool();
		if (tmpNoWait)
			newNoWait = tmpNoWait;

		bool tmpPaused = node.attribute(L"paused").as_bool();
		if (tmpPaused)
			newPaused = tmpPaused;

		return std::make_shared<ActionQueue>(newAction, newId, newGroups, newNoWait, newPaused, newTrigger);
	}

	std::shared_ptr<Animation> Parser::ParseAnimation(pugi::xml_node node)
	{
		int colSize = 0;
		int rowSize = 0;

		bool sliced = node.attribute(L"sliced").as_bool();

		std::wstring name = node.attribute(L"name").as_string(L"image");

		std::shared_ptr<AssetGraphic> sheet;
		std::wstring tmpSheet = node.attribute(L"sheet").as_string();

		if (!sliced)
		{
			sheet = AssetManager::GetGraphicByName(tmpSheet);
		}

		int x = node.attribute(L"x").as_int();
		int y = node.attribute(L"y").as_int();
		int length = node.attribute(L"length").as_int(1);

		int numCols = node.attribute(L"numCols").as_int(1);
		int numRows = node.attribute(L"numRows").as_int(1);

		int tmpColSize = node.attribute(L"colSize").as_int();
		if (tmpColSize)
			colSize = tmpColSize;
		else if (sheet)
			colSize = round(sheet->GetAsset()->getSize().x / length);

		int tmpRowSize = node.attribute(L"rowSize").as_int();
		if (tmpRowSize)
			rowSize = tmpRowSize;
		else if (sheet)
			rowSize = sheet->GetAsset()->getSize().y;

		int startPos = node.attribute(L"startPos").as_int();

		int frameInterval = node.attribute(L"frameInterval").as_int(1);
		int loopNum = node.attribute(L"loopNum").as_int(-1);
		std::wstring followUp = node.attribute(L"followUp").as_string();

		bool flipX = node.attribute(L"flipX").as_bool();
		bool flipY = node.attribute(L"flipY").as_bool();

		return std::make_shared<Animation>(name, tmpSheet, x, y, colSize, rowSize, startPos, length, std::to_wstring(frameInterval), loopNum, followUp, flipX, flipY, sliced, numCols, numRows);
	}

	std::shared_ptr<Character> Parser::ParseCharacter(pugi::xml_node node)
	{
		auto newChar = std::make_shared<Character>(std::wstring(node.attribute(L"name").as_string()),
												   node.attribute(L"x").as_int(),
												   node.attribute(L"y").as_int(),
												   node.attribute(L"width").as_int(),
												   node.attribute(L"height").as_int(),
												   node.attribute(L"sx").as_int(),
												   node.attribute(L"sy").as_int(),
												   node.attribute(L"sWidth").as_int(),
												   node.attribute(L"sHeight").as_int(),
												   node.attribute(L"sheet").as_string());

		std::wstring tmpFollowing = node.attribute(L"following").as_string();

		if (tmpFollowing != L"")
		{
			std::shared_ptr<Character> following = std::static_pointer_cast<Character>(Sburb::GetInstance()->GetSprite(tmpFollowing));
			if (following)
			{
				newChar->Follow(following);
			}
		}

		std::wstring tmpFollower = node.attribute(L"follower").as_string();
		if (tmpFollower != L"")
		{
			std::shared_ptr<Character> follower = std::static_pointer_cast<Character>(Sburb::GetInstance()->GetSprite(tmpFollower));
			if (follower)
			{
				follower->Follow(newChar);
			}
		}

		auto anims = GetNestedChildren(&node, L"animation");

		for (auto anim : anims)
		{
			std::shared_ptr<Animation> newAnim = ParseAnimation(anim);
			newChar->AddAnimation(newAnim);
		}
		newChar->StartAnimation(node.attribute(L"state").as_string());
		newChar->SetFacing(node.attribute(L"facing").as_string());

		return newChar;
	}

	Vector2 parse2Dimensions(std::wstring in)
	{
		std::vector<std::wstring> values = split(in, L",");
		Vector2 dimensions = Vector2();

		dimensions.x = stoi(values[0]);
		dimensions.y = stoi(values[1]);

		return dimensions;
	}

	Vector4 parse4Dimensions(std::wstring in)
	{
		std::vector<std::wstring> values = split(in, L",");
		Vector4 dimensions = Vector4();

		dimensions.x = stoi(values[0]);
		dimensions.y = stoi(values[1]);
		dimensions.z = stoi(values[2]);
		dimensions.w = stoi(values[3]);

		return dimensions;
	}

	std::shared_ptr<Dialoger> Parser::ParseDialoger(pugi::xml_node node)
	{
		Vector2 hiddenPos = parse2Dimensions(node.attribute(L"hiddenPos").as_string());
		Vector2 alertPos = parse2Dimensions(node.attribute(L"alertPos").as_string());
		Vector2 talkPosLeft = parse2Dimensions(node.attribute(L"talkPosLeft").as_string());
		Vector2 talkPosRight = parse2Dimensions(node.attribute(L"talkPosRight").as_string());
		Vector2 spriteStartRight = parse2Dimensions(node.attribute(L"spriteStartRight").as_string());
		Vector2 spriteEndRight = parse2Dimensions(node.attribute(L"spriteEndRight").as_string());
		Vector2 spriteStartLeft = parse2Dimensions(node.attribute(L"spriteStartLeft").as_string());
		Vector2 spriteEndLeft = parse2Dimensions(node.attribute(L"spriteEndLeft").as_string());
		Vector4 alertTextDimensions = parse4Dimensions(node.attribute(L"alertTextDimensions").as_string());
		Vector4 leftTextDimensions = parse4Dimensions(node.attribute(L"leftTextDimensions").as_string());
		Vector4 rightTextDimensions = parse4Dimensions(node.attribute(L"rightTextDimensions").as_string());
		std::wstring type = node.attribute(L"type").as_string(L"standard");

		auto newDialoger = std::make_shared<Dialoger>(hiddenPos, alertPos, talkPosLeft, talkPosRight,
													  spriteStartRight, spriteEndRight, spriteStartLeft, spriteEndLeft,
													  alertTextDimensions, leftTextDimensions, rightTextDimensions, type);

		std::wstring box = node.attribute(L"box").as_string();
		newDialoger->SetBox(box);

		return newDialoger;
	}

	std::shared_ptr<Fighter> Parser::ParseFighter(pugi::xml_node node)
	{
		std::wstring name = node.attribute(L"name").as_string();
		int x = node.attribute(L"x").as_int();
		int y = node.attribute(L"y").as_int();
		int width = node.attribute(L"width").as_int();
		int height = node.attribute(L"height").as_int();

		std::wstring newState = node.attribute(L"state").as_string();
		std::wstring newFacing = node.attribute(L"facing").as_string(L"Right");

		auto newSprite = std::make_shared<Fighter>(name, x, y, width, height);
		newSprite->SetFacing(newFacing);

		auto anims = GetNestedChildren(&node, L"animation");
		for (pugi::xml_node anim : anims)
		{
			std::shared_ptr<Animation> newAnim = ParseAnimation(anim);
			newSprite->AddAnimation(newAnim);

			if (newState == L"")
			{
				newState = newAnim->GetName();
			}
		}

		newSprite->StartAnimation(newState);

		return newSprite;
	}

	std::shared_ptr<Room> Parser::ParseRoom(pugi::xml_node node)
	{
		auto newRoom = std::make_shared<Room>(node.attribute(L"name").as_string(),
											  node.attribute(L"width").as_int(),
											  node.attribute(L"height").as_int());

		int mapScale = node.attribute(L"mapScale").as_int();
		if (mapScale != 0)
		{
			newRoom->SetMapScale(mapScale);
		}

		std::wstring walkableMap = node.attribute(L"walkableMap").as_string();
		if (walkableMap != L"")
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

		Serializer::SerialLoadRoomSprites(newRoom, GetNestedChildren(&node, L"sprite"));
		Serializer::SerialLoadRoomSprites(newRoom, GetNestedChildren(&node, L"character"));
		Serializer::SerialLoadRoomSprites(newRoom, GetNestedChildren(&node, L"fighter"));

		auto paths = node.child(L"paths");
		if (paths)
		{
			Serializer::SerialLoadRoomPaths(newRoom, paths);
		}

		auto triggers = node.child(L"triggers");
		if (triggers)
		{
			Serializer::SerialLoadRoomTriggers(newRoom, triggers);
		}

		return newRoom;
	}

	std::shared_ptr<Sprite> Parser::ParseSprite(pugi::xml_node node)
	{
		std::wstring name = node.attribute(L"name").as_string();
		int x = node.attribute(L"x").as_int();
		int y = node.attribute(L"y").as_int();
		int width = node.attribute(L"width").as_int();
		int height = node.attribute(L"height").as_int();
		int dx = node.attribute(L"dx").as_int();
		int dy = node.attribute(L"dy").as_int();
		int depthing = node.attribute(L"depthing").as_int();
		bool collidable = node.attribute(L"collidable").as_bool();
		std::wstring state = node.attribute(L"state").as_string();

		auto newSprite = std::make_shared<Sprite>(name, x, y, width, height, dx, dy, depthing, collidable);

		auto anims = GetNestedChildren(&node, L"animation");

		for (pugi::xml_node anim : anims)
		{
			std::shared_ptr<Animation> newAnim = ParseAnimation(anim);
			newSprite->AddAnimation(newAnim);

			if (state == L"")
			{
				state = newAnim->GetName();
			}
		}

		if (anims.empty())
		{
			std::shared_ptr<AssetGraphic> asset = AssetManager::GetGraphicByName(name);

			if (asset && asset->GetType() == L"graphic")
			{
				newSprite->AddAnimation(std::make_shared<Animation>(std::wstring(L"image"), name));
				state = L"image";
			}
		}

		newSprite->StartAnimation(state);

		return newSprite;
	}

	std::shared_ptr<SpriteButton> Parser::ParseSpriteButton(pugi::xml_node node)
	{
		std::shared_ptr<AssetGraphic> sheet = AssetManager::GetGraphicByName(node.attribute(L"sheet").as_string());

		auto newButton = std::make_shared<SpriteButton>(node.attribute(L"name").as_string(),
														node.attribute(L"x").as_int(),
														node.attribute(L"y").as_int(),
														node.attribute(L"width").as_int(sheet->GetAsset()->getSize().x),
														node.attribute(L"height").as_int(sheet->GetAsset()->getSize().y),
														node.attribute(L"sheet").as_string(),
														nullptr);

		auto action = node.child(L"action");
		if (action)
		{
			std::shared_ptr<Action> newAction = ParseAction(action);
			newButton->SetAction(newAction);
		}

		return newButton;
	}

	std::vector<std::wstring> GetTriggerNodeText(pugi::xml_node node)
	{
		std::vector<std::wstring> outputs = {};

		std::wostringstream serializeStream;

		for (pugi::xml_node child : node.children())
		{
			if (std::wstring(child.name()) == L"args")
			{
				for (pugi::xml_node subChild : child.children())
				{
					if (subChild.first_child())
					{
						std::wstring output = L"";
						for (pugi::xml_node subSubChild : child.children())
						{
							serializeStream.str(std::wstring());
							subSubChild.print(serializeStream, L"", pugi::format_raw);
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
			std::vector<std::wstring> info = GetTriggerNodeText(*curNode);
			for (int i = 0; i < info.size(); i++)
			{
				info[i] = unescape(info[i]);
			}

			auto action = GetNestedChild(curNode, L"action");

			std::shared_ptr<Action> curAction = nullptr;

			if (action && action.parent() == *curNode)
			{
				curAction = ParseAction(action);
			}

			bool restart = curNode->attribute(L"restart").as_bool();
			bool detonate = curNode->attribute(L"detonate").as_bool();
			std::wstring op = curNode->attribute(L"operator").as_string();

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

			auto child = GetNestedChild(curNode, L"trigger");

			if (child)
			{
				curNode = &child;
			}
			else
			{
				break;
			}
		} while (curNode);

		return firstTrigger;
	}
}