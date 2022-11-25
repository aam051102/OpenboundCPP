#ifndef SBURB_GAME_H
#define SBURB_GAME_H

#include "Common.h"
#include "Shader.h"
#include "Window.h"
#include "Input.h"
#include "Room.h"
#include "AssetManager.h"

#include <pugixml.hpp>

namespace SBURB
{
    // Handles a running instance of the game.
    class Game
    {
    public:
        Game();
        ~Game();
        static Game *GetInstance();

        void Update();
        void Render();

        bool Start();

        double GetFPS();
        std::string GetName();
        Room* GetRoom();

        void AddSprite(std::string name, std::shared_ptr<Sprite> sprite);

        void PerformAction(std::shared_ptr<Action> action);

        void SetMouseCursor(std::string cursor);

        Window window;
        Camera *camera;
        Shader shaderProgram;
        AssetManager assetManager;

        std::string name;
        std::string levelPath;
        std::string version;
        std::string resourcePath;

    private:
        std::map<std::string, std::shared_ptr<Sprite>> sprites;

        InputHandler inputHandler;

        sf::Image icon;

        sf::Int32 FPS;
        sf::Clock FPStimeObj;

        Room *room;
    };
}

#endif
