#include "Fighter.h"

namespace SBURB {
    Fighter::Fighter(std::string name, int x, int y, int width, int height) : 
		Sprite(name, x, y, width, height, 0, 0, Depth::MG_DEPTHING, true)
	{
		this->accel = 1.5;
		this->decel = 1;
		this->friction = 0.87;
		this->vx = 0;
		this->vy = 0;
		this->facing = "Right";
    }

    Fighter::~Fighter() {

    }

	void Fighter::Update() {
		this->TryToMove();
		this->Sprite::Update();
		this->animation->SetFlipX(this->facing == "Left");
	}

	void Fighter::HandleInputs(std::map<InputActions, bool> pressed) {
		bool moved = false;
		if (pressed[InputActions::Down]) {
			this->MoveDown(); moved = true;
		}
		else if (pressed[InputActions::Up]) {
			this->MoveUp(); moved = true;
		}

		if (pressed[InputActions::Left]) {
			this->MoveLeft(); moved = true;
		}
		else if (pressed[InputActions::Right]) {
			this->MoveRight(); moved = true;
		}

		if (pressed[InputActions::Attack]) {
			this->Attack();
		}

		if (!moved) {
			this->Idle();
		}
	}
}