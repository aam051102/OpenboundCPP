#include "SpriteButton.h"
#include "Sburb.h"
#include <AssetHandler.h>

namespace SBURB {
    SpriteButton::SpriteButton(std::string name, int x, int y, int width, int height, std::string sheetName, std::shared_ptr<Action> action) :
        Sprite(name, x, y, width, height) {
        this->pressed = false;
        this->mousePressed = false;
        this->clicked = false;
        this->action = action;

        this->sheet = AssetHandler::GetTextureByName(sheetName);

        for (int i = 0; i < (sheet->getSize().x / this->width) * (sheet->getSize().y / this->height); i++) {
            this->AddAnimation(std::make_shared<Animation>("state" + i, sheetName, 0, 0, width, height, i, 1, "1000"));
        }

        this->StartAnimation("state0");
    }

    SpriteButton::~SpriteButton() {

    }

    void SpriteButton::Update() {
        this->Sprite::Update();
        this->UpdateMouse();
    }

    void SpriteButton::UpdateMouse() {
		int x = InputHandler::GetMousePosition().x;
		int y = InputHandler::GetMousePosition().y;
		int mouseDown = InputHandler::IsMousePressed(MouseInputActions::Click);

		this->clicked = false;
		if (this->HitsPoint(x - this->width / 2, y - this->height / 2)) {
			Sburb::GetInstance()->SetMouseCursor(sf::Cursor::Hand);
		}
		if (mouseDown) {
			if (!this->mousePressed) {
				this->mousePressed = true;
				if (this->HitsPoint(x - this->width / 2, y - this->height / 2)) {
					this->pressed = true;
				}
			}
		}
		else {
			if (this->pressed) {
				if (this->HitsPoint(x - this->width / 2, y - this->height / 2)) {
					this->clicked = true;
					std::string nextState = "state" + (stoi(this->animation->GetName().substr(5)) + 1);
					
					if (this->animations[nextState]) {
						this->StartAnimation(nextState);
					}
					else {
						this->StartAnimation("state0");
					}
				}
			}
		
			this->pressed = false;
			this->mousePressed = false;
		}

		if (this->clicked && this->action) {
			Sburb::GetInstance()->PerformAction(this->action);
		}
    }

	void SpriteButton::SetState(std::string state) {
		this->StartAnimation("state" + state);
	}

	std::string SpriteButton::Serialize(std::string output) {
		output = output + "\n<spritebutton name='" + this->name +
			(this->x ? "' x='" + std::to_string(this->x) : "") +
			(this->y ? "' y='" + std::to_string(this->y) : "") +
			"' width='" + std::to_string(this->width) +
			"' height='" + std::to_string(this->height) +
			"' sheet='" + this->animation->GetSheet()->GetName() +
			"' >";

		if (this->action) {
			output = this->action->Serialize(output);
		}

		output = output + "</spritebutton>";
		return output;
	}
}