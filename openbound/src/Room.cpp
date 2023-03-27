#include "Room.h"

constexpr int BLOCK_SIZE = 500;

namespace SBURB
{
	Room::Room(std::wstring name, int width, int height)
	{
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

	Room::~Room()
	{
	}

	sf::Vector2f Room::GetAdjustedMovement(Sprite *sprite, float ax, float ay)
	{
		for (int i = 0; i < this->motionPaths.size(); i++)
		{
			std::shared_ptr<MotionPath> motionPath = this->motionPaths[i];
			bool shouldMove = motionPath->path->Query(sf::Vector2f(sprite->GetX(), sprite->GetY()));

			if (shouldMove)
			{
				float fx = (ax * motionPath->xtox + ay * motionPath->ytox + motionPath->dx);
				float fy = (ax * motionPath->xtoy + ay * motionPath->ytoy + motionPath->dy);
				return sf::Vector2f(fx, fy);
			}
		}

		return sf::Vector2f(ax, ay);
	}

	sf::Vector2f Room::GetInverseAdjustedMovement(Sprite *sprite, float ax, float ay)
	{
		for (int i = 0; i < this->motionPaths.size(); i++)
		{
			std::shared_ptr<MotionPath> motionPath = this->motionPaths[i];
			bool shouldMove = motionPath->path->Query(sf::Vector2f(sprite->GetX(), sprite->GetY()));

			if (shouldMove)
			{
				ax -= motionPath->dx;
				ay -= motionPath->dy;

				float fx, fy;
				float det = motionPath->xtox * motionPath->ytoy - motionPath->xtoy * motionPath->ytox;

				if (det)
				{
					fx = (ax * motionPath->ytoy - ay * motionPath->ytox) / det;
					fy = (-ax * motionPath->xtoy + ay * motionPath->xtox) / det;
					return sf::Vector2f(fx, fy);
				}
				else
				{
					return sf::Vector2f(0, 0);
				}
			}
		}

		return sf::Vector2f(ax, ay);
	}

	void Room::AddEffect(std::shared_ptr<Animation> effect)
	{
		this->effects.push_back(effect);
	}

	void Room::AddTrigger(std::shared_ptr<Trigger> trigger)
	{
		this->triggers.push_back(trigger);
	}

	void Room::AddSprite(std::shared_ptr<Sprite> sprite)
	{
		if (!this->Contains(sprite))
		{
			this->sprites.push_back(sprite);
		}
	}

	bool Room::RemoveSprite(std::shared_ptr<Sprite> sprite)
	{
		for (int i = 0; i < this->sprites.size(); i++)
		{
			if (this->sprites[i] == sprite)
			{
				this->sprites.erase(this->sprites.begin() + i);
				return true;
			}
		}

		return false;
	}

	void Room::AddWalkable(std::shared_ptr<AssetPath> path)
	{
		this->walkables.push_back(path);
	}

	void Room::RemoveWalkable(std::shared_ptr<AssetPath> path)
	{
		this->walkables.erase(std::find(this->walkables.begin(), this->walkables.end(), path));
	}

	void Room::AddUnwalkable(std::shared_ptr<AssetPath> path)
	{
		this->unwalkables.push_back(path);
	}

	void Room::RemoveUnwalkable(std::shared_ptr<AssetPath> path)
	{
		this->unwalkables.erase(std::find(this->unwalkables.begin(), this->unwalkables.end(), path));
	}

	void Room::AddMotionPath(std::shared_ptr<AssetPath> path, float xtox, float xtoy, float ytox, float ytoy, float dx, float dy)
	{
		this->motionPaths.push_back(std::make_shared<MotionPath>(path, xtox, xtoy, ytox, ytoy, dx, dy));
	}

	void Room::Enter()
	{
		if (this->walkableMap)
		{
			sf::Image img = this->walkableMap->GetAsset()->copyToImage();
			this->mapData = std::make_shared<sf::Image>(img);
		}
	}

	void Room::Exit()
	{
		this->effects.clear();
		this->mapData = nullptr;
	}

	bool Room::Contains(std::shared_ptr<Sprite> sprite)
	{
		for (int i = 0; i < this->sprites.size(); i++)
		{
			if (this->sprites[i].get() == sprite.get())
			{
				return true;
			}
		}

		return false;
	}

	void Room::Update()
	{
		for (auto sprite : this->sprites)
		{
			sprite->Update();
		}

		for (int i = this->effects.size() - 1; i >= 0; i--)
		{
			if (this->effects[i]->HasPlayed())
			{
				this->effects.erase(this->effects.begin() + i);
			}
			else
			{
				this->effects[i]->Update();
			}
		}

		for (int i = this->triggers.size() - 1; i >= 0; i--)
		{
			if (this->triggers[i]->TryToTrigger())
			{
				this->triggers.erase(this->triggers.begin() + i);
			}
		}

		this->SortDepths(); // Moved here from draw due to const issue. If issues occur, refer to source code.
	}

	void Room::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		for (int i = 0; i < this->sprites.size(); i++)
		{
			target.draw(*this->sprites[i], states);
		}

		for (int i = 0; i < this->effects.size(); i++)
		{
			target.draw(*this->effects[i], states);
		}
	}

	void Room::SortDepths()
	{
		// Could probably be replaced with an existing sort method.
		for (int i = 1, j = 1; i < this->sprites.size(); i++, j = i)
		{
			std::shared_ptr<Sprite> temp = this->sprites[j];

			while (j > 0 && temp->IsBehind(this->sprites[j - 1]))
			{
				this->sprites[j] = this->sprites[j - 1];
				j--;
			}

			this->sprites[j] = temp;
		}
	}

	std::vector<std::shared_ptr<Action>> Room::QueryActions(std::shared_ptr<Sprite> query, float x, float y)
	{
		std::vector<std::shared_ptr<Action>> validActions = {};

		for (int i = 0; i < this->sprites.size(); i++)
		{
			std::shared_ptr<Sprite> sprite = this->sprites[i];

			if (sprite != query && sprite->HitsPoint(x, y))
			{
				std::vector<std::shared_ptr<Action>> actions = sprite->GetActions(query);

				validActions.insert(validActions.end(), actions.begin(), actions.end());
			}
		}

		return validActions;
	}

	std::vector<std::shared_ptr<Action>> Room::QueryActionsVisual(std::shared_ptr<Sprite> query, int x, int y)
	{
		std::vector<std::shared_ptr<Action>> validActions = {};

		for (int i = 0; i < this->sprites.size(); i++)
		{
			std::shared_ptr<Sprite> sprite = this->sprites[i];

			if (sprite->IsVisuallyUnder(x, y))
			{
				std::vector<std::shared_ptr<Action>> actions = sprite->GetActions(query);

				validActions.insert(validActions.end(), actions.begin(), actions.end());
			}
		}

		return validActions;
	}

	bool Room::IsInBounds(Sprite *sprite, int dx, int dy)
	{
		std::map<std::wstring, sf::Vector2f> queries = sprite->GetBoundaryQueries(dx, dy);
		std::map<std::wstring, bool> results;
		this->IsInBoundsBatch(queries, &results);

		for (auto point : results)
		{
			if (!point.second)
			{
				return false;
			}
		}

		return true;
	}

	std::map<std::wstring, bool> Room::IsInBoundsBatch(std::map<std::wstring, sf::Vector2f> queries, std::map<std::wstring, bool> *results)
	{
		if (this->walkableMap)
		{
			for (auto query : queries)
			{
				sf::Vector2f pt = query.second;
				int width = this->walkableMap->GetAsset()->getSize().x;
				int height = this->walkableMap->GetAsset()->getSize().y;

				if (pt.x < 0 || pt.x > width * this->mapScale || pt.y < 0 || pt.y > height * this->mapScale)
				{
					(*results)[query.first] = false;
				}
				else
				{
					int mapX = round(pt.x / (float)this->mapScale);
					int mapY = round(pt.y / (float)this->mapScale);

					(*results)[query.first] = this->mapData->getPixel(mapX, mapY) == sf::Color::White;
				}
			}
		}

		for (int i = 0; i < this->walkables.size(); i++)
		{
			this->walkables[i]->QueryBatchPos(queries, results);
		}

		for (int i = 0; i < this->unwalkables.size(); i++)
		{
			this->unwalkables[i]->QueryBatchNeg(queries, results);
		}

		return *results;
	}

	std::shared_ptr<Sprite> Room::Collides(Sprite *sprite, float dx, float dy)
	{
		for (int i = 0; i < this->sprites.size(); i++)
		{
			std::shared_ptr<Sprite> curSprite = this->sprites[i];

			if (curSprite->GetCollidable() && sprite != curSprite.get())
			{
				if (sprite->Collides(curSprite, dx, dy))
				{
					return curSprite;
				}
			}
		}

		return nullptr;
	}

	std::wstring Room::Serialize(std::wstring output)
	{
		output = output + L"\n<room name='" + this->name +
				 L"' width='" + std::to_wstring(this->width) +
				 L"' height='" + std::to_wstring(this->height) +
				 (this->walkableMap ? (L"' walkableMap='" + this->walkableMap->GetName()) : L"") +
				 (this->mapScale != 4 ? (L"' mapScale='" + std::to_wstring(this->mapScale)) : L"") +
				 L"' >";

		output = output + L"\n<paths>";

		for (int i = 0; i < this->walkables.size(); i++)
		{
			std::shared_ptr<AssetPath> walkable = this->walkables[i];
			output = output + L"\n<walkable path='" + walkable->GetName() + L"'/>";
		}

		for (int i = 0; i < this->unwalkables.size(); i++)
		{
			std::shared_ptr<AssetPath> unwalkable = this->unwalkables[i];
			output = output + L"\n<unwalkable path='" + unwalkable->GetName() + L"'/>";
		}

		for (int i = 0; i < this->motionPaths.size(); i++)
		{
			std::shared_ptr<MotionPath> motionPath = this->motionPaths[i];
			output = output + L"\n<motionpath path='" + motionPath->path->GetName() + L"' xtox='" + std::to_wstring(motionPath->xtox) + L"' xtoy='" + std::to_wstring(motionPath->xtoy) +
					 L"' ytox='" + std::to_wstring(motionPath->ytox) + L"' ytoy='" + std::to_wstring(motionPath->ytoy) + L"' dx='" + std::to_wstring(motionPath->dx) + L"' dy='" + std::to_wstring(motionPath->dy) + L"'/>";
		}

		output = output + L"\n</paths>";
		output = output + L"\n<triggers>";

		for (int i = 0; i < this->triggers.size(); i++)
		{
			output = this->triggers[i]->Serialize(output);
		}

		output = output + L"\n</triggers>";

		for (int i = 0; i < this->sprites.size(); i++)
		{
			output = this->sprites[i]->Serialize(output);
		}

		output = output + L"\n</room>";

		return output;
	}
}
