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
