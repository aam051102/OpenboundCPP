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
#include "Path.h"

namespace SBURB
{
    static Game* gameInstance = nullptr;

    Game::Game()
    {
        this->name = "Jterniabound";
        this->version = "1.0";
        this->icon = sf::Image();

        this->FPS = 30;
        this->FPStimeObj = sf::Clock();

        this->room = nullptr;
        this->camera = nullptr;

        this->globalObjects = {};
        this->assetManager = AssetManager();

        if (gameInstance == nullptr) {
            gameInstance = this;
        }
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
        return gameInstance->GetRoomInternal();
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
        if (resourcePath != "") {
            this->resourcePath = resourcePath;
        }

        std::string name = rootNode.attribute("name").value();
        if(name != "") this->name = name;
        
        std::string version = rootNode.attribute("version").value();
        if(version != "") this->version = version;
       
        std::string width = rootNode.attribute("width").value();
        if (width != "") {
            this->window.SetSize({stoi(width), this->window.GetSize().y});
        }

        std::string height = rootNode.attribute("height").value();
        if (height != "") {
            this->window.SetSize({  this->window.GetSize().y, stoi(height) });
        }

        LoadDependencies(rootNode);
        LoadSerialAssets(rootNode);

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

    bool Game::LoadSerialAssets(pugi::xml_node node) {
        std::string description = node.attribute("description").value();
        if (description != "") {
            this->assetManager.SetDescription(description);
        }
        else {
            this->assetManager.SetDescription("assets");
        }

        pugi::xml_node assetsNode = node.child("assets");

        if (assetsNode) {
            auto assetNodes = assetsNode.children("asset");

            for (pugi::xml_node assetNode : assetNodes) {
                if (!this->assetManager.CheckIsLoaded(assetNode.attribute("name").value())) {
                    LoadSerialAsset(assetNode);
                }
            }
        }

        return true;
    }

    bool Game::LoadSerialAsset(pugi::xml_node node) {
        auto newAsset = ParseSerialAsset(node);
        this->assetManager.LoadAsset(newAsset);
        return true;
    }

    Asset Game::ParseSerialAsset(pugi::xml_node node) {
        std::string name = node.attribute("name").value();
        std::string type = node.attribute("type").value();
        std::string value = trim(node.text().as_string());

        std::string blobUrlsAttr = node.attribute("blob-urls").value();
        std::vector<std::string> blobUrls = {};

        if (blobUrlsAttr != "") {
            blobUrls = split(blobUrlsAttr, ";");
        }

        Asset asset;
        if (type == "graphic") {
            asset = CreateGraphicAsset(name, value, blobUrls);
        }
        else if(type == "audio") {
            asset = CreateAudioAsset(name, split(value, ";"), blobUrls);
        }
        else if (type == "path") {
            std::vector<std::string> points = split(value, ";");
            Path path = Path();

            for (auto point : points) {
                auto pointCoords = split(point, ",");
                path.Push({ stoi(pointCoords[0]), stoi(pointCoords[1]) });
            }

            asset = CreatePathAsset(name, path);
        }
        else if (type == "movie") {
            asset = CreateMovieAsset(name, value, blobUrls);
        }
        else if (type == "font") {
            asset = CreateFontAsset(name, value);
        }
        else if (type == "text") {
            asset = CreateTextAsset(name, value);
        }
        
        return asset;
    }

    bool Game::Start()
    {
        // Create & initialize main window
        window.Init(name, { 650, 450 }, sf::Style::Close | sf::Style::Titlebar, icon); // Standard

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
        gameInstance->LoadRoomInternal(roomId);
    }

    void Game::LoadRoom(Room *room)
    {
        gameInstance->LoadRoomInternal(room);
    }

    // Getters
    double Game::GetFPS()
    {
        return this->FPS;
    }

    std::string Game::GetName()
    {
        return name;
    }

    Room *Game::GetRoom()
    {
        return this->room;
    }

    Game *Game::GetInstance()
    {
        return gameInstance;
    }
}
