#include <sstream>
#include <algorithm>
#include "Game.h"
#include "Resources.h"
#include "Logger.h"
#include "Sprite.h"
#include "AssetHandler.h"
#include "BatchHandler.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

static inline std::string trim(std::string s) {
    const int len = s.length();

    int start = 0;
    int end = len - 1;

    for (int i = 0; i < len; i++) {
        if (s[i] != ' ') {
            start = i;
            break;
        }
    }

    for (int i = len - 1; i >= start; i--) {
        if (s[i] != ' ') {
            end = i;
            break;
        }
    }

    std::string newStr = "";

    for (int i = start; i <= end; i++) {
        newStr += s[i];
    }

    return newStr;
}

namespace SBURB
{
    static Game *instance;

    Game::Game()
    {
        this->title = "Openbound";
        this->icon = sf::Image();

        this->FPS = 30;
        this->FPStimeObj = sf::Clock();

        this->room = nullptr;
        this->camera = nullptr;

        this->globalObjects = {};

        instance = this;
    }

    Game::~Game()
    {
        AssetHandler::ClearTextures();
    }

    void Game::Update()
    {
        sf::Int32 FPStime = FPStimeObj.getElapsedTime().asMilliseconds();

        // Event polling
        sf::Event event;
        while (window->pollEvent(event))
        {
            inputHandler.Update(event, !window->hasFocus());

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                    window->close();
            }
            else if (event.type == sf::Event::Closed)
            {
                window->close();
            }
        }

        // FPS check
        if (FPStime >= FPS)
        {
            double delta = FPStimeObj.restart().asSeconds();
            const double deltaCalculations = 1.0 / ((double)FPS - 10);
            if (delta > deltaCalculations)
            {
                delta = deltaCalculations;
            }

            // Run main update method for all objects
            if (room != nullptr) {
                std::vector<Object*> objects = room->GetObjects();
                objects.reserve(objects.size() + globalObjects.size() + 1);
                objects.insert(objects.end(), globalObjects.begin(), globalObjects.end());
                for (int i = 0; i < objects.size(); i++)
                {
                    objects[i]->Update((float)delta);
                }
            }

            if (camera != nullptr) {
                camera->Update();
                window->setView(*camera->GetView());
            }

            // Render
            Render();
        }
    }

    void Game::Render()
    {
        window->clear(sf::Color(0, 0, 0, 255));
        shaderProgram.Bind();

        BatchHandler::getInstance().Reset();

        // Render all objects
        if (room != nullptr) {
            std::vector<Object*> objects = room->GetObjects();
            objects.reserve(objects.size() + globalObjects.size() + 1);
            objects.insert(objects.end(), globalObjects.begin(), globalObjects.end());
            std::sort(objects.begin(), objects.end(), [](const Object* x, const Object* y)
                { return x->depth < y->depth; });

            sf::View windowView = window->getView();
            for (int i = 0; i < objects.size(); i++)
            {
                if (std::find(UIObjects.begin(), UIObjects.end(), objects[i]) != UIObjects.end())
                {
                    window->setView(window->getDefaultView());
                    window->draw(*objects[i]);

                    if (BatchHandler::getInstance().BatchExists())
                        BatchHandler::getInstance().DrawBatch();
                    window->setView(windowView);
                }
                else
                {
                    window->draw(*objects[i]);
                }
            }
        }

        if (BatchHandler::getInstance().BatchExists())
            BatchHandler::getInstance().DrawBatch();

        window->display();
    }

    Room *Game::GetRoomInternal()
    {
        return room;
    }

    Room *Game::GetRoomStatic()
    {
        return instance->GetRoomInternal();
    }

    bool Game::LoadSerial(std::string path) {
        pugi::xml_document doc;
        pugi::xml_parse_result initDocRes = doc.load_file(path.c_str());

        if (initDocRes.status != pugi::status_ok) {
            std::string errMsg = "For " + path + ": " + initDocRes.description();
            GlobalLogger->Log(Logger::Error, errMsg);
            return false;
        }

        pugi::xml_node rootNode = doc.child("sburb");

        std::string levelPath = rootNode.attribute("levelPath").value();
        if (levelPath != "") {
            if (levelPath[levelPath.length() - 1] == '/') {
                this->levelPath = levelPath;
            }
            else {
                this->levelPath = levelPath + "/";
            }
        }
        
        std::string resourcePath = rootNode.attribute("resourcePath").value();
        std::string name = rootNode.attribute("name").value();
        std::string version = rootNode.attribute("version").value();
        std::string width = rootNode.attribute("width").value();
        std::string height = rootNode.attribute("height").value();
        
        LoadDependencies(rootNode);

        return true;
    }

    bool Game::LoadDependencies(pugi::xml_node node) {
        pugi::xml_node dependenciesNode = node.child("dependencies");

        if (dependenciesNode) {
            auto dependencyNodes = dependenciesNode.children("dependency");

            for (pugi::xml_node dependencyNode : dependencyNodes) {
                auto dependencyPath = dependencyNode.text().as_string();

                LoadSerial(this->levelPath + trim(dependencyPath));
            }

        }

        return true;
    }

    bool Game::Start()
    {
        // Create & initialize main window
        window.Init(title, { 650, 450 }, sf::Style::Close | sf::Style::Titlebar, icon); // Standard

        if (!window.GetWin())
        {
            GlobalLogger->Log(Logger::Error, "Failed to create main game window.");
            return false;
        }

        // Center window
        window.CenterWindow();

        // Initialize handlers
        collisionHandler.Reset();
        saveHandler.Reset();

        // Initialize room
        if(room != nullptr) room->Initialize();

        if (!LoadSerial("./levels/init.xml")) return false;

        // Start update loop
        while (window->isOpen())
        {
            Update();
        }

        return true;
    }

    void Game::LoadRoomInternal(Room *room)
    {
        if (this->room == room)
            return;
        this->room = room;
        if (!this->room->IsInitialized())
        {
            room->Initialize();
        }
    }

    void Game::LoadRoomInternal(int roomId)
    {
        if (this->room == tempRoomList[roomId])
            return;
        if (roomId == -1)
            return;
        this->room = tempRoomList[roomId];
        if (!this->room->IsInitialized())
        {
            room->Initialize();
        }
    }

    void Game::LoadRoom(int roomId)
    {
        instance->LoadRoomInternal(roomId);
    }

    void Game::LoadRoom(Room *room)
    {
        instance->LoadRoomInternal(room);
    }

    // Getters
    double Game::GetFPS()
    {
        return this->FPS;
    }

    std::string Game::GetTitle()
    {
        return title;
    }

    Room *Game::GetRoom()
    {
        return this->room;
    }

    Game *Game::GetInstance()
    {
        return instance;
    }
}
