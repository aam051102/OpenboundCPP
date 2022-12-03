#ifndef SBURB_ROOM_H
#define SBURB_ROOM_H

#include "Common.h"
#include "Animation.h"
#include "AssetPath.h"
#include "Trigger.h"
#include "Sprite.h"

namespace SBURB
{
	struct MotionPath {
		std::shared_ptr<AssetPath> path;
		int xtox;
		int xtoy;
		int ytox;
		int ytoy;
		int dx;
		int dy;

		MotionPath(std::shared_ptr<AssetPath> path, int xtox, int xtoy, int ytox, int ytoy, int dx, int dy) : path(path), xtox(xtox), xtoy(xtoy), ytox(ytox), ytoy(ytoy), dx(dx), dy(dy) {};
	};

    class Room : public sf::Drawable
    {
    public:
        Room(std::string name, int width, int height);
		~Room();

		void AddEffect(std::shared_ptr<Animation> effect);
		void AddTrigger(std::shared_ptr<Trigger> trigger);

		void AddSprite(std::shared_ptr<Sprite> sprite);
		bool RemoveSprite(std::shared_ptr<Sprite> sprite);

		void AddMotionPath(std::shared_ptr<AssetPath> path, int xtox, int xtoy, int ytox, int ytoy, int dx, int dy);

		void AddWalkable(std::shared_ptr<AssetPath> path);
		void RemoveWalkable(std::shared_ptr<AssetPath> path);

		void AddUnwalkable(std::shared_ptr<AssetPath> path);
		void RemoveUnwalkable(std::shared_ptr<AssetPath> path);

		void Enter();
		void Exit();

		bool Contains(std::shared_ptr<Sprite> sprite);

		void Update();

		void SortDepths();

		std::vector<std::shared_ptr<Action>> QueryActions(std::shared_ptr<Sprite> query, int x, int y);
		std::vector<std::shared_ptr<Action>> QueryActionsVisual(std::shared_ptr<Sprite> query, int x, int y);

		bool IsInBounds(std::shared_ptr<Sprite> sprite, int dx = 0, int dy = 0);
		std::map<std::string, bool> IsInBoundsBatch(std::map<std::string, Vector2> queries, std::map<std::string, bool>* results = nullptr);

		std::shared_ptr<Sprite> Collides(std::shared_ptr<Sprite> sprite, int dx = 0, int dy = 0);

		std::string Serialize(std::string output);

		std::string GetName() { return this->name; };

		void SetMapScale(int mapScale) { this->mapScale = mapScale; };
		int GetMapScale() { return this->mapScale; };

		void SetWalkableMap(std::shared_ptr<AssetGraphic> walkableMap) { this->walkableMap = walkableMap; };
		std::shared_ptr<AssetGraphic> GetWalkableMap() { return this->walkableMap; };

		void SetWidth(int width) { this->width = width; };
		int GetWidth() { return this->width; };

		void SetHeight(int height) { this->height = height; };
		int GetHeight() { return this->height; };

    private:
		std::string name;
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
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}

#endif
