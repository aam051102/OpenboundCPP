#ifndef SBURB_ROOM_H
#define SBURB_ROOM_H

#include "Common.h"
#include "Animation.h"
#include "AssetPath.h"
#include "Trigger.h"
#include "Sprite.h"

namespace SBURB
{
	struct MotionPath
	{
		std::shared_ptr<AssetPath> path;
		float xtox;
		float xtoy;
		float ytox;
		float ytoy;
		float dx;
		float dy;

		MotionPath(std::shared_ptr<AssetPath> path, float xtox, float xtoy, float ytox, float ytoy, float dx, float dy) : path(path), xtox(xtox), xtoy(xtoy), ytox(ytox), ytoy(ytoy), dx(dx), dy(dy){};
	};

	class Room : public sf::Drawable
	{
	public:
		Room(std::wstring name, int width, int height);
		~Room();

		sf::Vector2f Room::GetAdjustedMovement(Sprite *sprite, float ax, float ay);
		sf::Vector2f Room::GetInverseAdjustedMovement(Sprite *sprite, float ax, float ay);

		void AddEffect(std::shared_ptr<Animation> effect);
		void AddTrigger(std::shared_ptr<Trigger> trigger);

		void AddSprite(std::shared_ptr<Sprite> sprite);
		bool RemoveSprite(std::shared_ptr<Sprite> sprite);

		void AddMotionPath(std::shared_ptr<AssetPath> path, float xtox, float xtoy, float ytox, float ytoy, float dx, float dy);

		void AddWalkable(std::shared_ptr<AssetPath> path);
		void RemoveWalkable(std::shared_ptr<AssetPath> path);

		void AddUnwalkable(std::shared_ptr<AssetPath> path);
		void RemoveUnwalkable(std::shared_ptr<AssetPath> path);

		void Enter();
		void Exit();

		bool Contains(std::shared_ptr<Sprite> sprite);

		void Update();

		void SortDepths();

		std::vector<std::shared_ptr<Action>> QueryActions(std::shared_ptr<Sprite> query, float x, float y);
		std::vector<std::shared_ptr<Action>> QueryActionsVisual(std::shared_ptr<Sprite> query, int x, int y);

		bool IsInBounds(Sprite *sprite, int dx = 0, int dy = 0);
		std::map<std::wstring, bool> IsInBoundsBatch(std::map<std::wstring, sf::Vector2f> queries, std::map<std::wstring, bool> *results);

		std::shared_ptr<Sprite> Collides(Sprite *sprite, float dx = 0, float dy = 0);

		std::wstring Serialize(std::wstring output);

		std::wstring GetName() { return this->name; };

		void SetMapScale(int mapScale) { this->mapScale = mapScale; };
		int GetMapScale() { return this->mapScale; };

		void SetWalkableMap(std::shared_ptr<AssetGraphic> walkableMap) { this->walkableMap = walkableMap; };
		std::shared_ptr<AssetGraphic> GetWalkableMap() { return this->walkableMap; };

		void SetWidth(int width) { this->width = width; };
		int GetWidth() { return this->width; };

		void SetHeight(int height) { this->height = height; };
		int GetHeight() { return this->height; };

	private:
		std::wstring name;
		int width;
		int height;
		std::vector<std::shared_ptr<Sprite>> sprites;
		std::vector<std::shared_ptr<Animation>> effects;
		std::vector<std::shared_ptr<AssetPath>> walkables;
		std::vector<std::shared_ptr<AssetPath>> unwalkables;
		std::vector<std::shared_ptr<MotionPath>> motionPaths;
		std::vector<std::shared_ptr<Trigger>> triggers;
		std::shared_ptr<AssetGraphic> walkableMap;
		std::shared_ptr<sf::Image> mapData;
		int mapScale;

	private:
		virtual void draw(sf::RenderTarget &target, const sf::RenderStates& states) const;
	};
}

#endif
