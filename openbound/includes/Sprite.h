#ifndef SBURB_SPRITE_H
#define SBURB_SPRITE_H
#include <SFML/Graphics.hpp>

#include "Common.h"
#include "Texture.h"
#include "Animation.h"
#include "Action.h"
#include "Room.h"

namespace SBURB
{
    struct BoundaryQueries {
        Vector2 upLeft = Vector2(0, 0);
        Vector2 upMid = Vector2(0, 0);
        Vector2 upRight = Vector2(0, 0);
        Vector2 downLeft = Vector2(0, 0);
        Vector2 downMid = Vector2(0, 0);
        Vector2 downRight = Vector2(0, 0);

        BoundaryQueries() {  };
    };

    class Sprite : public sf::Drawable, public sf::Transformable
    {
    public:
        Sprite(std::string name, int x, int y, int width, int height, int dx, int dy, int depthing, bool collidable);

        void AddAnimation(std::shared_ptr<Animation> anim);
        void StartAnimation(std::string name);
        void Update();
        
        bool IsBehind(std::shared_ptr<Sprite> other);
        bool Collides(std::shared_ptr<Sprite> other, int dx, int dy);
        bool HitsPoint(int x, int y);
        bool IsVisuallyUnder(int x, int y);
        
        void AddAction(std::shared_ptr<Action> action);
        void RemoveAction(std::string name);
        std::vector<std::shared_ptr<Action>> GetActions(std::shared_ptr<Sprite> sprite);

        BoundaryQueries GetBoundaryQueries(int dx, int dy);

        Sprite Clone(std::string name);
        std::string Serialize(std::string output);

        std::string GetName() { return this->name; };

        sf::IntRect textureRect;
        int textureId;

    protected:
        std::string name;
        int x;
        int y;
        int width;
        int height;
        int dx;
        int dy;
        int depthing;
        bool collidable;
        std::map<std::string, std::shared_ptr<Animation>> animations;
        std::shared_ptr<Animation> animation;
        std::string state;
        int lastTime;
        std::map<std::string, std::shared_ptr<Action>> actions;
        BoundaryQueries queries;

    private:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    };
}

#endif