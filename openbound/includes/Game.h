#ifndef SBURB_GAME_H
#define SBURB_GAME_H

#include "Common.h"
#include "Shader.h"
#include "Window.h"
#include "Input.h"
#include "Room.h"
#include "Asset.h"
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

        void Update();
        void Render();

        Room *GetRoomInternal();
        static Room *GetRoomStatic();

        bool Start();

        void LoadRoomInternal(Room *room);

        inline void AddGlobalObject(Object *object) { globalObjects.push_back(object); }
        inline void AddUIObject(Object *object) { UIObjects.push_back(object); }

        // TODO: Replace with a method to load the room
        // from our binary file format.
        void LoadRoomInternal(int roomId);

        static void LoadRoom(int roomId);
        static void LoadRoom(Room *room);

        double GetFPS();
        std::string GetName();
        Room *GetRoom();

        static Game *GetInstance();

        void AddSprite(std::string name, std::shared_ptr<Sprite> sprite);

        // TODO: Replace this map into a map of memory locations
        // in our binary file format.
        std::map<unsigned int, Room *> tempRoomList;

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

        std::vector<Object *> globalObjects;
        std::vector<Object *> UIObjects;

        Room *room;
    };
}

#endif
