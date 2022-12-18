#ifndef SBURB_SPRITE_H
#define SBURB_SPRITE_H

#include <SFML/Graphics.hpp>
#include "Common.h"
#include "AssetGraphic.h"
#include "Animation.h"
#include "Action.h"
#include "Room.h"

namespace SBURB
{
    enum class Depth : int {
        BG_DEPTHING = 0,
        MG_DEPTHING = 1,
        FG_DEPTHING = 2
    };

    class Sprite : public sf::Drawable, public sf::Transformable
    {
    public:
        Sprite(std::string name = "", int x = 0, int y = 0, int width = 0, int height = 0, int dx = 0, int dy = 0, int depthing = static_cast<int>(Depth::BG_DEPTHING), bool collidable = false);

        void AddAnimation(std::shared_ptr<Animation> anim);
        void StartAnimation(std::string name);
        std::map<std::string, std::shared_ptr<Animation>> GetAnimations() { return this->animations; };
        std::shared_ptr<Animation> GetAnimation() { return this->animation; };
        std::shared_ptr<Animation> GetAnimation(std::string name) { return this->animations[name]; };
        virtual void Update();
        
        bool IsBehind(std::shared_ptr<Sprite> other);
        bool Collides(std::shared_ptr<Sprite> other, int dx, int dy);
        bool HitsPoint(int x, int y);
        bool IsVisuallyUnder(int x, int y);
        
        void AddAction(std::shared_ptr<Action> action);
        void RemoveAction(std::string name);
        std::vector<std::shared_ptr<Action>> GetActions(std::shared_ptr<Sprite> sprite);

        std::map<std::string, sf::Vector2f> GetBoundaryQueries(int dx, int dy);

        std::shared_ptr<Sprite> Clone(std::string name);
        std::string Serialize(std::string output);

        std::string GetName() { return this->name; };
        bool GetCollidable() { return this->collidable; };
        int GetWidth() { return this->width; };
        int GetHeight() { return this->height; };

        void SetDepthing(int depthing) { this->depthing = depthing; };

        void SetX(int x) { this->x = x; this->setPosition(this->x, this->y); };
        int GetX() { return this->x; };

        void SetY(int y) { this->y = y; this->setPosition(this->x, this->y); };
        int GetY() { return this->y; };

        std::string GetProp(std::string prop);

    protected:
        std::string name;
        float x;
        float y;
        int width;
        int height;
        float dx;
        float dy;
        int depthing;
        bool collidable;
        std::map<std::string, std::shared_ptr<Animation>> animations;
        std::shared_ptr<Animation> animation;
        std::string state;
        int lastTime;
        std::vector<std::shared_ptr<Action>> actions;
        std::map<std::string, sf::Vector2f> queries;

    private:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    };
}

#endif