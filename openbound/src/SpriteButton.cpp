#include "SpriteButton.h"
#include "Sburb.h"
#include <AssetManager.h>

namespace SBURB
{
	SpriteButton::SpriteButton(std::string name, int x, int y, int width, int height, std::string sheetName, std::shared_ptr<Action> action) : Sprite(name, x, y, width, height)
	{
		this->pressed = false;
		this->mousePressed = false;
		this->clicked = false;
		this->action = action;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->sheetName = sheetName;

		this->sheet = AssetManager::GetGraphicByName(sheetName);
		auto sheetAsset = this->sheet->Load();

		for (int i = 0, l = (sheetAsset->getSize().x / this->width) * (sheetAsset->getSize().y / this->height); i < l; i++)
		{
			this->AddAnimation(std::make_shared<Animation>("state" + std::to_string(i), sheetName, 0, 0, width, height, i, 1, "1000"));
		}

		this->sheet->Unload();

		this->StartAnimation("state0");
		this->setPosition(x, y);
	}

	SpriteButton::~SpriteButton()
	{
	}

	void SpriteButton::Update()
	{
		this->Sprite::Update();
		this->UpdateMouse();
	}

	void SpriteButton::Load()
	{
		if (this->animation)
		{
			this->animation->Load();
		}

		if (this->sheet) {
			this->sheet->Load();
		}
	}

	void SpriteButton::Unload()
	{
		if (this->animation)
		{
			this->animation->Unload();
		}

		if (this->sheet) {
			this->sheet->Unload();
		}
	}

	void SpriteButton::UpdateMouse()
	{
		int x = InputHandler::GetMousePosition().x;
		int y = InputHandler::GetMousePosition().y;
		int mouseDown = InputHandler::GetMouseDown();

		this->clicked = false;
		if (this->HitsPoint(x - this->width / 2, y - this->height / 2))
		{
			Sburb::GetInstance()->SetMouseCursor(sf::Cursor::Hand);
		}
		if (mouseDown)
		{
			if (!this->mousePressed)
			{
				this->mousePressed = true;
				if (this->HitsPoint(x - this->width / 2, y - this->height / 2))
				{
					this->pressed = true;
				}
			}
		}
		else
		{
			if (this->pressed)
			{
				if (this->HitsPoint(x - this->width / 2, y - this->height / 2))
				{
					this->clicked = true;
					std::string nextState = "state" + std::to_string(stoi(this->animation->GetName().substr(5)) + 1);

					if (this->animations[nextState])
					{
						this->StartAnimation(nextState);
					}
					else
					{
						this->StartAnimation("state0");
					}
				}
			}

			this->pressed = false;
			this->mousePressed = false;
		}

		if (this->clicked && this->action)
		{
			Sburb::GetInstance()->PerformAction(this->action);
		}
	}

	void SpriteButton::SetState(std::string state)
	{
		this->StartAnimation("state" + state);
	}

	std::string SpriteButton::Serialize(std::string output)
	{
		output = output + "\n<spritebutton name='" + this->name +
				 (this->x ? "' x='" + std::to_string(this->x) : "") +
				 (this->y ? "' y='" + std::to_string(this->y) : "") +
				 "' width='" + std::to_string(this->width) +
				 "' height='" + std::to_string(this->height) +
				 "' sheet='" + this->animation->GetSheet()->GetName() +
				 "' >";

		if (this->action)
		{
			output = this->action->Serialize(output);
		}

		output = output + "</spritebutton>";
		return output;
	}
}