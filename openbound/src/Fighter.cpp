#include "Fighter.h"
#include "Serializer.h"
#include "Sburb.h"

constexpr float PI = 3.14;

namespace SBURB {
    Fighter::Fighter(std::string name, int x, int y, int width, int height) : 
		Sprite(name, x, y, width, height, 0, 0, static_cast<int>(Depth::MG_DEPTHING), true)
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

	void Fighter::HandleInputs(std::map<sf::Keyboard::Key, bool> pressed) {
		bool moved = false;
		if (pressed[sf::Keyboard::Down] || pressed[sf::Keyboard::S]) {
			this->MoveDown(); moved = true;
		}
		else if (pressed[sf::Keyboard::Up] || pressed[sf::Keyboard::W]) {
			this->MoveUp(); moved = true;
		}

		if (pressed[sf::Keyboard::Left] || pressed[sf::Keyboard::A]) {
			this->MoveLeft(); moved = true;
		}
		else if (pressed[sf::Keyboard::Right] || pressed[sf::Keyboard::D]) {
			this->MoveRight(); moved = true;
		}

		if (pressed[sf::Keyboard::Space] || pressed[sf::Keyboard::Enter] || pressed[sf::Keyboard::LControl]) {
			this->Attack();
		}

		if (!moved) {
			this->Idle();
		}
	}

	void Fighter::Idle() {
		if (this->state == "walk") {
			this->StartAnimation("idle");
		}
	}

	void Fighter::Walk() {
		if (this->state == "idle") {
			this->StartAnimation("walk");
		}
	}

	void Fighter::Attack() {
		this->StartAnimation("attack");
	}

	void Fighter::MoveUp() {
		this->Walk();
		this->vy -= this->accel;
	}

	void Fighter::MoveDown() {
		this->Walk();
		this->vy += this->accel;
	}

	void Fighter::MoveLeft() {
		this->Walk();
		this->vx -= this->accel;
		this->facing = "Left";
	}
	
	void Fighter::MoveRight() {
		this->Walk();
		this->vx += this->accel;
		this->facing = "Right";
	}

	void Fighter::MoveNone() {

	}

	void Fighter::BecomePlayer() {

	}

	void Fighter::BecomeNPC() {

	}

	std::vector<int> Fighter::GetActionQueries() {
		std::vector<int> queries = {};
		return queries;
	}

	bool Fighter::Collides(std::shared_ptr<Sprite> sprite, int dx, int dy) {
		if (!this->width || !sprite->GetWidth()) {
			return false;
		}
		int x1 = this->x + dx;
		int y1 = this->y + dy;
		int w1 = this->width / 2;
		int h1 = this->height / 2;

		int x2 = sprite->GetX();
		int y2 = sprite->GetY();
		int w2 = sprite->GetWidth() / 2;
		int h2 = sprite->GetHeight() / 2;

		int xDiff = x2 - x1;
		int yDiff = y2 - y1;
		return sqrt(xDiff * xDiff / w2 / w1 + yDiff * yDiff / h2 / h1) < 2;
	}

	std::map<std::string, sf::Vector2f> Fighter::GetBoundaryQueries(int dx, int dy) {
		int x = this->x + dx;
		int y = this->y + dy;
		int queryCount = 8;
		int angleDiff = 2 * PI / queryCount;

		std::map<std::string, sf::Vector2f> queries = {};
		
		for (int i = 0, theta = 0; i < queryCount; i++, theta += angleDiff) {
			queries[std::to_string(i)] = sf::Vector2f(x + cos(theta) * this->width / 2, y + sin(theta) * this->height / 2);
		}

		return queries;
	}

	bool Fighter::TryToMove() {
		this->vx *= this->friction;
		this->vy *= this->friction;

		if (abs(this->vx) < this->decel) {
			this->vx = 0;
		}
		if (abs(this->vy) < this->decel) {
			this->vy = 0;
		}

		float vx = this->vx;
		float vy = this->vy;

		// NOTE: Ignore moveMap for now
		//var moveMap = room.getMoveFunction(this);
		/*if (moveMap) { //our motion could be modified somehow
			l = moveMap(vx, vy);
			vx = l.x;
			vy = l.y;
		}*/
		float dx = vx;
		float dy = vy;
		this->x += vx;
		this->y += vy;

		std::shared_ptr<Room> room = Sburb::GetInstance()->GetCurrentRoom();

		std::shared_ptr<Sprite> collides = room->Collides(this);
		if (collides) {
			float tx = 0;
			float ty = 0;
			float theta = atan2(this->y - collides->GetY(), this->x - collides->GetX());
			float xOff = cos(theta);
			float yOff = sin(theta);
			while (this->Collides(collides, tx, ty)) {
				tx -= (dx - xOff) * 0.1;
				ty -= (dy - yOff) * 0.1;
			}
			if (room->Collides(this, tx, ty)) {
				this->x -= dx;
				this->y -= dy;
				return false;
			}
			this->x += tx;
			this->y += ty;
			dx += tx;
			dy += ty;

			theta = atan2(this->y - collides->GetY(), this->x - collides->GetX());
			this->vx += tx;
			this->vy += ty;
			this->vx *= 0.9;
			this->vy *= 0.9;
		}

		std::map<std::string, bool> queries;
		room->IsInBoundsBatch(this->GetBoundaryQueries(), &queries);

		float queryCount = 8;
		bool collided = false;
		float hitX = 0;
		float hitY = 0;
		float angleDiff = 2 * PI / queryCount;

		float theta = 0;
		int currentQuery = 0;

		for (auto query : queries) {
			if (currentQuery >= queryCount) break;

			if (!query.second) {
				hitX += cos(theta);
				hitY += sin(theta);
				collided = true;
			}

			theta += angleDiff;
			currentQuery++;
		}

		if (collided) {
			float tx = 0;
			float ty = 0;
			float theta = atan2(hitY, hitX);
			float xOff = cos(theta);
			float yOff = sin(theta);
			int timeout = 0;

			while (!room->IsInBounds(this, tx, ty) && timeout < 20) {
				tx -= xOff * 2;
				ty -= yOff * 2;
				timeout++;
			}
			
			if (timeout >= 20 || room->Collides(this, tx, ty)) {
				this->x -= dx;
				this->y -= dy;
				return false;
			}

			this->x += tx;
			this->y += ty;
			dx += tx;
			dy += ty;

			this->vx += tx;
			this->vy += ty;
			this->vx *= 0.9;
			this->vy *= 0.9;
		}

		return true;
	}

	std::string Fighter::Serialize(std::string output) {
		int animationCount = 0;
		for (auto anim : this->animations) {
			animationCount++;
		}

		output = output + "<fighter " +
			Serializer::SerializeAttribute("name", this->name) +
			Serializer::SerializeAttribute("x", (int)this->x) +
			Serializer::SerializeAttribute("y", (int)this->y) +
			Serializer::SerializeAttribute("width", this->width) +
			Serializer::SerializeAttribute("height", this->height) + 
			Serializer::SerializeAttribute("facing", this->facing) +
			(animationCount > 1 ? "state='" + this->state + "' " : "") +
			">";

		for (auto anim : this->animations) {
			output = anim.second->Serialize(output);
		}

		for (auto action : this->actions) {
			output = action->Serialize(output);
		}
		
		output = output + "</fighter>";
		
		return output;
	}
}