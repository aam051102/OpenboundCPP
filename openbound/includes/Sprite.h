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
        Sprite(std::wstring name = L"", int x = 0, int y = 0, int width = 0, int height = 0, int dx = 0, int dy = 0, int depthing = static_cast<int>(Depth::BG_DEPTHING), bool collidable = false);

        void AddAnimation(std::shared_ptr<Animation> anim);
        void StartAnimation(std::wstring name);
        std::map<std::wstring, std::shared_ptr<Animation>> GetAnimations() { return this->animations; };
        std::shared_ptr<Animation> GetAnimation() { return this->animation; };
        std::shared_ptr<Animation> GetAnimation(std::wstring name) { return this->animations[name]; };
        virtual void Update();
        
        bool IsBehind(std::shared_ptr<Sprite> other);
        bool Collides(std::shared_ptr<Sprite> other, float dx, float dy);
        bool HitsPoint(float x, float y);
        bool IsVisuallyUnder(int x, int y);
        
        void AddAction(std::shared_ptr<Action> action);
        void RemoveAction(std::wstring name);
        std::vector<std::shared_ptr<Action>> GetActions(std::shared_ptr<Sprite> sprite);

        std::map<std::wstring, sf::Vector2f> GetBoundaryQueries(int dx, int dy);

        std::shared_ptr<Sprite> Clone(std::wstring name);
        std::wstring Serialize(std::wstring output);

        std::wstring GetName() { return this->name; };
        bool GetCollidable() { return this->collidable; };
        int GetWidth() { return this->width; };
        int GetHeight() { return this->height; };

        void SetDepthing(int depthing) { this->depthing = depthing; };

        void SetX(int x) { this->x = x; this->setPosition(sf::Vector2f(this->x, this->y)); };
        int GetX() { return this->x; };

        void SetY(int y) { this->y = y; this->setPosition(sf::Vector2f(this->x, this->y)); };
        int GetY() { return this->y; };

        std::wstring GetProp(std::wstring prop);

    protected:
        std::wstring name;
        float x;
        float y;
        int width;
        int height;
        float dx;
        float dy;
        int depthing;
        bool collidable;
        std::map<std::wstring, std::shared_ptr<Animation>> animations;
        std::shared_ptr<Animation> animation;
        std::wstring state;
        int lastTime;
        std::vector<std::shared_ptr<Action>> actions;
        std::map<std::wstring, sf::Vector2f> queries;

    private:
        virtual void draw(sf::RenderTarget &target, const sf::RenderStates& states) const override;
    };
}

#endif