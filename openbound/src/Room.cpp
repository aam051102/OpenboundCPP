#include "Room.h"

constexpr int BLOCK_SIZE = 500;

namespace SBURB
{
    Room::Room(std::string name, int width, int height) {
		this->name = name;
		this->width = width;
		this->height = height;
		this->sprites = {};
		this->effects = {};
		this->walkables = {};
		this->unwalkables = {};
		this->motionPaths = {};
		this->triggers = {};
		this->walkableMap = "";
		this->mapScale = 4;
    }

	Room::~Room() {

	}

	void Room::AddEffect(std::shared_ptr<Animation> effect) {
		this->effects.push_back(effect);
	}

	void Room::AddTrigger(std::shared_ptr<Trigger> trigger) {
		this->triggers.push_back(trigger);
	}

	void Room::AddSprite(std::shared_ptr<Sprite> sprite) {
		if (!this>Contains(sprite)) {
			this->sprites.push_back(sprite);
		}
	}

	bool Room::RemoveSprite(std::shared_ptr<Sprite> sprite) {
		for (int i = 0; i < this->sprites.size(); i++) {
			if (this->sprites[i] == sprite) {
				this->sprites.erase(this->sprites.begin() + i);
				return true;
			}
		}

		return false;
	}

	void Room::AddWalkable(std::shared_ptr<Path> path) {
		this->walkables.push_back(path);
	}

	void Room::RemoveWalkable(std::shared_ptr<Path> path) {
		this->walkables.erase(std::find(this->walkables.begin(), this->walkables.end(), path));
	}
	
	void Room::AddUnwalkable(std::shared_ptr<Path> path) {
		this->unwalkables.push_back(path);
	}

	void Room::RemoveUnwalkable(std::shared_ptr<Path> path) {
		this->unwalkables.erase(std::find(this->unwalkables.begin(), this->unwalkables.end(), path));
	}

	void Room::AddMotionPath(std::shared_ptr<Path> path, int xtox, int xtoy, int ytox, int ytoy, int dx, int dy) {
		MotionPath motionPath = MotionPath(path, xtox, xtoy, ytox, ytoy, dx, dy);

		this->motionPaths.push_back(std::make_shared<MotionPath>(motionPath));
	}

	void Room::Enter() {
		// TODO
	}

	void Room::Exit() {
		this->effects.clear();
		// TODO: Clear mapdata?
	}

	bool Room::Contains(std::shared_ptr<Sprite> sprite) {
		for (int i = 0; i < this->sprites.size(); i++) {
			if (this->sprites[i] == sprite) {
				return true;
			}
		}

		return false;
	}

	void Room::Update() {
		for (auto sprite : this->sprites) {
			sprite->Update();
		}

		for (int i = this->effects.size() - 1; i >= 0; i--) {
			if (this->effects[i]->HasPlayed()) {
				this->effects.erase(this->effects.begin() + i);
			}
			else {
				this->effects[i]->Update();
			}
		}

		for (int i = this->triggers.size() - 1; i >= 0; i--) {
			if (this->triggers[i]->TryToTrigger()) {
				this->triggers.erase(this->triggers.begin() + i);
			}
		}

		this->SortDepths(); // Moved here from draw due to const issue. If issues occur, refer to source code.
	}

	void Room::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		for (int i = 0; i < this->sprites.size(); i++) {
			target.draw(*this->sprites[i], states);
		}

		for (int i = 0; i < this->effects.size(); i++) {
			target.draw(*this->effects[i], states);
		}
	}

	void Room::SortDepths() {
		// Could probably be replaced with an existing sort method.
		for (int i = 1, int j = 1; i < this->sprites.size(); i++, j = i) {
			std::shared_ptr<Sprite> temp = this->sprites[j];
			
			while (j > 0 && temp->IsBehind(this->sprites[j - 1])) {
				this->sprites[j] = this->sprites[j - 1];
				j--;
			}

			this->sprites[j] = temp;
		}
	}

	std::vector<std::shared_ptr<Action>> Room::QueryActions(std::shared_ptr<Sprite> query, int x, int y) {
		std::vector<std::shared_ptr<Action>> validActions = {};

		for (int i = 0; i < this->sprites.size(); i++) {
			std::shared_ptr<Sprite> sprite = this->sprites[i];
			if (sprite != query && sprite->HitsPoint(x, y)) {
				std::vector<std::shared_ptr<Action>> actions = sprite->GetActions(query);

				validActions.insert(validActions.end(), actions.begin(), actions.end());
			}
		}

		return validActions;
	}

	std::vector<std::shared_ptr<Action>> Room::QueryActionsVisual(std::shared_ptr<Sprite> query, int x, int y) {
		std::vector<std::shared_ptr<Action>> validActions = {};

		for (int i = 0; i < this->sprites.size(); i++) {
			std::shared_ptr<Sprite> sprite = this->sprites[i];
			if (sprite->IsVisuallyUnder(x, y)) {
				std::vector<std::shared_ptr<Action>> actions = sprite->GetActions(query);

				validActions.insert(validActions.end(), actions.begin(), actions.end());
			}
		}

		return validActions;
	}

	bool Room::IsInBounds(std::shared_ptr<Sprite> sprite, int dx, int dy) {
		std::map<std::string, Vector2> queries = sprite->GetBoundaryQueries(dx, dy);
		std::map<std::string, bool> result = this->IsInBoundsBatch(queries);

		for (auto point : result) {
			if (!point.second) {
				return false;
			}
		}

		return true;
	}

	std::map<std::string, bool> Room::IsInBoundsBatch(std::map<std::string, Vector2> queries, std::map<std::string, bool>* results) {
		if (results == nullptr) {
			results = &std::map<std::string, bool>();
		}

		if (this->walkableMap != "") {
			for (auto query : queries) {
				Vector2 pt = query.second;
				int width = this->walkableMap.width;
				int height = this->walkableMap.height;
				
				if (pt.x<0 || pt.x>width * this->mapScale || pt.y<0 || pt.y>height * this->mapScale) {
					(*results)[query.first] = false;
				}
				else {
					int imgPt = (round(pt.x / this->mapScale) + round(pt.y / this->mapScale) * width) * 4;
					(*results)[query.first] = !!this->mapData[imgPt];
				}
			}
		}

		for (int i = 0; i < this->walkables.size(); i++) {
			this->walkables[i]->QueryBatchPos(queries, results);
		}

		for (int i = 0; i < this->unwalkables.size(); i++) {
			this->unwalkables[i]->QueryBatchNeg(queries, results);
		}

		return *results;
	}

	std::shared_ptr<Sprite> Room::Collides(std::shared_ptr<Sprite> sprite, int dx, int dy) {
		for (int i = 0; i < this->sprites.size(); i++) {
			std::shared_ptr<Sprite> curSprite = this->sprites[i];
		
			if (curSprite->GetCollidable() && sprite != curSprite) {
				if (sprite->Collides(curSprite, dx, dy)) {
					return curSprite;
				}
			}
		}

		return nullptr;
	}
}
