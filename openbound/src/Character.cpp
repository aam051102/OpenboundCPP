#include "Character.h"

constexpr int FOLLOW_BUFFER_LENGTH = 6;

namespace SBURB {
    Character::Character(std::string name, int x, int y, int width, int height, int sx, int sy, int sWidth, int sHeight, std::string sheetName, bool bootstrap):
		Sprite(name, x, y, width, height, 0, 0, Depth::MG_DEPTHING, true) {
		this->speed = 12;
		this->vx = 0;
		this->vy = 0;
		this->facing = "Front";
		this->npc = true;
		this->spriteType = "character";
		this->following = nullptr;
		this->followBuffer = {};
		this->follower = nullptr;
		this->lastLeaderPos = Vector2();
		this->handledInput = -1;
		this->oldX = 0;
		this->oldY = 0;

		if (!bootstrap) { // Automagically generate standard animations
			this->AddAnimation(std::make_shared<Animation>("idleFront", sheetName, sx, sy, sWidth, sHeight, 0, 1, "2"));
			this->AddAnimation(std::make_shared<Animation>("idleRight", sheetName, sx, sy, sWidth, sHeight, 1, 1, "2"));
			this->AddAnimation(std::make_shared<Animation>("idleBack", sheetName, sx, sy, sWidth, sHeight, 2, 1, "2"));
			this->AddAnimation(std::make_shared<Animation>("idleLeft", sheetName, sx, sy, sWidth, sHeight, 3, 1, "2"));
			this->AddAnimation(std::make_shared<Animation>("walkFront", sheetName, sx, sy, sWidth, sHeight, 4, 2, "4"));
			this->AddAnimation(std::make_shared<Animation>("walkRight", sheetName, sx, sy, sWidth, sHeight, 6, 2, "4"));
			this->AddAnimation(std::make_shared<Animation>("walkBack", sheetName, sx, sy, sWidth, sHeight, 8, 2, "4"));
			this->AddAnimation(std::make_shared<Animation>("walkLeft", sheetName, sx, sy, sWidth, sHeight, 10, 2, "4"));


			this->StartAnimation("walkFront");
		}
		else {
			this->bootstrap = true;
		}

		this->BecomeNPC();
    }

    Character::~Character() {

    }

	void Character::Update() {
		this->HandleFollowing();

		// what does this code block do????
		// NOTE: These were originally this->handleInput, but the variable was never set, so I made the assumption that they were supposed to be this->handledInput. If you find issues with idling, it may be here.
		if (this->handledInput > 0) {
			--this->handledInput;
			if (this->handledInput == 0) {
				this->MoveNone();
			}
		}

		this->TryToMove(this->vx, this->vy);

		this->Sprite::Update();
	}
	
	void Character::HandleFollowing() {
		if (this.following) {
			if (this.following.isNPC() && !this.isNPC()) {
				this.becomeNPC();
				this.collidable = true;
				this.walk();
			}
			else if (!this.following.isNPC() && this.isNPC()) {
				this.becomePlayer();
				this.collidable = false;
			}

			if (this.following.x != this.lastLeaderPos.x || this.following.y != this.lastLeaderPos.y) {
				this.followBuffer.push({ x:this.following.x,y : this.following.y });
				this.lastLeaderPos.x = this.following.x;
				this.lastLeaderPos.y = this.following.y;
			}

			var destPos = null;

			while (this->followBuffer.size() > FOLLOW_BUFFER_LENGTH) {
				destPos = this->followBuffer[0];

				std::vector<std::string> keys = {};

				/*if (moveMap) {
					delta = moveMap(destPos.x - this.x, destPos.y - this.y);
				}
				else {*/
				// NOTE: Got rid of moveMap for now.
				Vector2 delta = Vector2(destPos.x - this->x, destPos.y - this->y);
				//}

				if (abs(delta.x) >= this->speed / 1.9) {
					if (delta.x > 0) {
						keys.push_back(Sburb.Keys.right);
					}
					else {
						keys.push_back(Sburb.Keys.left);
					}
				}
				if (abs(delta.y) >= this->speed / 1.9) {
					if (delta.y > 0) {
						keys.push_back(Sburb.Keys.down);
					}
					else {
						keys.push_back(Sburb.Keys.up);
					}
				}
				if (keys.size()  == 0) {
					this->followBuffer.erase(this->followBuffer.begin() + 0);
					continue;
				}
				else {
					this->HandleInputs(keys);
				}
				break;
			}

			if (this->followBuffer.size()  <= FOLLOW_BUFFER_LENGTH && !this->following->IsNPC()) {
				if (destPos) {
					this->x = destPos.x;
					this->y = destPos.y;
				}

				this->MoveNone();
			}
		}
	}
}