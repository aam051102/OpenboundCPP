#ifndef SBURB_SPRITE_H
#define SBURB_SPRITE_H
#include <SFML/Graphics.hpp>

#include "Common.h"
#include "AssetTexture.h"
#include "Animation.h"
#include "Action.h"
#include "Room.h"

namespace SBURB
{
    enum class Depth {
        BG_DEPTHING = 0,
        MG_DEPTHING = 1,
        FG_DEPTHING = 2
    };

    class Sprite : public sf::Drawable, public sf::Transformable
    {
    public:
        Sprite(std::string name, int x, int y, int width, int height, int dx = 0, int dy = 0, Depth depthing = Depth::BG_DEPTHING, bool collidable = false);

        void AddAnimation(std::shared_ptr<Animation> anim);
        void StartAnimation(std::string name);
        std::shared_ptr<Animation> GetAnimation() { return this->animation; };
        void Update();
        
        bool IsBehind(std::shared_ptr<Sprite> other);
        bool Collides(std::shared_ptr<Sprite> other, int dx, int dy);
        bool HitsPoint(int x, int y);
        bool IsVisuallyUnder(int x, int y);
        
        void AddAction(std::shared_ptr<Action> action);
        void RemoveAction(std::string name);
        std::vector<std::shared_ptr<Action>> GetActions(std::shared_ptr<Sprite> sprite);

        std::map<std::string, Vector2> GetBoundaryQueries(int dx, int dy);

        Sprite Clone(std::string name);
        std::string Serialize(std::string output);

        std::string GetName() { return this->name; };
        bool GetCollidable() { return this->collidable; };
        int GetWidth() { return this->width; };
        int GetHeight() { return this->height; };

        void SetX(int x) { this->x = x; };
        int GetX() { return this->x; };

        void SetY(int y) { this->y = y; };
        int GetY() { return this->y; };

        std::string GetProp(std::string prop);

    protected:
        std::string name;
        int x;
        int y;
        int width;
        int height;
        int dx;
        int dy;
        Depth depthing;
        bool collidable;
        std::map<std::string, std::shared_ptr<Animation>> animations;
        std::shared_ptr<Animation> animation;
        std::string state;
        int lastTime;
        std::map<std::string, std::shared_ptr<Action>> actions;
        std::map<std::string, Vector2> queries;

    private:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    };
}

#endif