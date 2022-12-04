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
		this->walkableMap = nullptr;
		this->mapData = nullptr;
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
		if (!this->Contains(sprite)) {
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

	void Room::AddWalkable(std::shared_ptr<AssetPath> path) {
		this->walkables.push_back(path);
	}

	void Room::RemoveWalkable(std::shared_ptr<AssetPath> path) {
		this->walkables.erase(std::find(this->walkables.begin(), this->walkables.end(), path));
	}
	
	void Room::AddUnwalkable(std::shared_ptr<AssetPath> path) {
		this->unwalkables.push_back(path);
	}

	void Room::RemoveUnwalkable(std::shared_ptr<AssetPath> path) {
		this->unwalkables.erase(std::find(this->unwalkables.begin(), this->unwalkables.end(), path));
	}

	void Room::AddMotionPath(std::shared_ptr<AssetPath> path, int xtox, int xtoy, int ytox, int ytoy, int dx, int dy) {
		MotionPath motionPath = MotionPath(path, xtox, xtoy, ytox, ytoy, dx, dy);

		this->motionPaths.push_back(std::make_shared<MotionPath>(motionPath));
	}

	void Room::Enter() {
		if (this->walkableMap) {
			sf::Image img = this->walkableMap->GetAsset()->copyToImage();
			this->mapData = std::make_shared<sf::Image>(img);
		}
	}

	void Room::Exit() {
		this->effects.clear();
		this->mapData = nullptr;
	}

	bool Room::Contains(std::shared_ptr<Sprite> sprite) {
		for (int i = 0; i < this->sprites.size(); i++) {
			if (this->sprites[i].get() == sprite.get()) {
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
		for (int i = 1, j = 1; i < this->sprites.size(); i++, j = i) {
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

	bool Room::IsInBounds(Sprite* sprite, int dx, int dy) {
		std::map<std::string, Vector2> queries = sprite->GetBoundaryQueries(dx, dy);
		std::map<std::string, bool> results;
		this->IsInBoundsBatch(queries, &results);

		for (auto point : results) {
			if (!point.second) {
				return false;
			}
		}

		return true;
	}

	std::map<std::string, bool> Room::IsInBoundsBatch(std::map<std::string, Vector2> queries, std::map<std::string, bool>* results) {
		if (this->walkableMap) {
			for (auto query : queries) {
				Vector2 pt = query.second;
				int width = this->walkableMap->GetAsset()->getSize().x;
				int height = this->walkableMap->GetAsset()->getSize().y;
				
				if (pt.x<0 || pt.x>width * this->mapScale || pt.y<0 || pt.y>height * this->mapScale) {
					(*results)[query.first] = false;
				}
				else {
					int mapX = round(pt.x / this->mapScale);
					int mapY = round(pt.y / this->mapScale);

					(*results)[query.first] = this->mapData->getPixel(mapX, mapY) == sf::Color::White;
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

	std::shared_ptr<Sprite> Room::Collides(Sprite* sprite, int dx, int dy) {
		for (int i = 0; i < this->sprites.size(); i++) {
			std::shared_ptr<Sprite> curSprite = this->sprites[i];
		
			if (curSprite->GetCollidable() && sprite != curSprite.get()) {
				if (sprite->Collides(curSprite, dx, dy)) {
					return curSprite;
				}
			}
		}

		return nullptr;
	}
	
	std::string Room::Serialize(std::string output) {
		output = output + "\n<room name='" + this->name +
			"' width='" + std::to_string(this->width) +
			"' height='" + std::to_string(this->height) +
			(this->walkableMap ? ("' walkableMap='" + this->walkableMap->GetName()) : "") +
			(this->mapScale != 4 ? ("' mapScale='" + std::to_string(this->mapScale)) : "") +
			"' >";

		output = output + "\n<paths>";

		for (int i = 0; i < this->walkables.size(); i++) {
			std::shared_ptr<AssetPath> walkable = this->walkables[i];
			output = output + "\n<walkable path='" + walkable->GetName() + "'/>";
		}

		for (int i = 0; i < this->unwalkables.size(); i++) {
			std::shared_ptr<AssetPath> unwalkable = this->unwalkables[i];
			output = output + "\n<unwalkable path='" + unwalkable->GetName() + "'/>";
		}

		for (int i = 0; i < this->motionPaths.size(); i++) {
			std::shared_ptr<MotionPath> motionPath = this->motionPaths[i];
			output = output + "\n<motionpath path='" + motionPath->path->GetName()  + "' xtox='" + std::to_string(motionPath->xtox) + "' xtoy='" + std::to_string(motionPath->xtoy) +
				"' ytox='" + std::to_string(motionPath->ytox) + "' ytoy='" + std::to_string(motionPath->ytoy) + "' dx='" + std::to_string(motionPath->dx) + "' dy='" + std::to_string(motionPath->dy) + "'/>";
		}

		output = output + "\n</paths>";
		output = output + "\n<triggers>";

		for (int i = 0; i < this->triggers.size(); i++) {
			output = this->triggers[i]->Serialize(output);
		}

		output = output + "\n</triggers>";

		for (int i = 0; i < this->sprites.size(); i++) {
			output = this->sprites[i]->Serialize(output);
		}

		output = output + "\n</room>";

		return output;
	}
}
