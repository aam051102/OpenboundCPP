#include "SpriteButton.h"
#include "Sburb.h"
#include <AssetManager.h>

namespace SBURB
{
	SpriteButton::SpriteButton(std::wstring name, int x, int y, int width, int height, std::wstring sheetName, std::shared_ptr<Action> action) : Sprite(name, x, y, width, height)
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

		for (int i = 0; i < (sheet->GetAsset()->getSize().x / this->width) * (sheet->GetAsset()->getSize().y / this->height); i++)
		{
			this->AddAnimation(std::make_shared<Animation>(L"state" + std::to_wstring(i), sheetName, 0, 0, width, height, i, 1, L"1000"));
		}

		this->StartAnimation(L"state0");
		this->setPosition(sf::Vector2f(x, y));
	}

	SpriteButton::~SpriteButton()
	{
	}

	void SpriteButton::Update()
	{
		this->Sprite::Update();
		this->UpdateMouse();
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
					std::wstring nextState = L"state" + std::to_wstring(stoi(this->animation->GetName().substr(5)) + 1);

					if (this->animations.find(nextState) != this->animations.end())
					{
						this->StartAnimation(nextState);
					}
					else
					{
						this->StartAnimation(L"state0");
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

	void SpriteButton::SetState(std::wstring state)
	{
		this->StartAnimation(L"state" + state);
	}

	std::wstring SpriteButton::Serialize(std::wstring output)
	{
		output = output + L"\n<spritebutton name='" + this->name +
				 (this->x ? L"' x='" + std::to_wstring(this->x) : L"") +
				 (this->y ? L"' y='" + std::to_wstring(this->y) : L"") +
				 L"' width='" + std::to_wstring(this->width) +
				 L"' height='" + std::to_wstring(this->height) +
				 L"' sheet='" + this->animation->GetSheet()->GetName() +
				 L"' >";

		if (this->action)
		{
			output = this->action->Serialize(output);
		}

		output = output + L"</spritebutton>";
		return output;
	}
}