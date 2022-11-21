#ifndef SBURB_GAME_H
#define SBURB_GAME_H

#include "Common.h"
#include "Shader.h"
#include "Window.h"
#include "Input.h"
#include "Object.h"
#include "Room.h"
#include "CollisionHandler.h"
#include "SaveHandler.h"
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

        bool LoadSerial(std::string path);
        bool LoadDependencies(pugi::xml_node node);
        bool LoadSerialAssets(pugi::xml_node node);
        bool LoadSerialAsset(pugi::xml_node node);

        Asset ParseSerialAsset(pugi::xml_node node);

        // TODO: Replace this map into a map of memory locations
        // in our binary file format.
        std::map<unsigned int, Room *> tempRoomList;

        Window window;
        Camera *camera;
        Shader shaderProgram;
        AssetManager assetManager;

    private:
        InputHandler inputHandler;

        std::string name;
        std::string version;
        std::string levelPath;
        std::string resourcePath;


        sf::Image icon;

        sf::Int32 FPS;
        sf::Clock FPStimeObj;

        std::vector<Object *> globalObjects;
        std::vector<Object *> UIObjects;

        Room *room;

        CollisionHandler collisionHandler;
        SaveHandler saveHandler;
    };
}

#endif
