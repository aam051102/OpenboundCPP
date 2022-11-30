#include <sstream>
#include <algorithm>
#include "Sburb.h"
#include "Logger.h"
#include "Sprite.h"
#include "AssetHandler.h"
#include "BatchHandler.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Path.h"
#include "Serializer.h"
#include "Parser.h"

namespace SBURB
{
    static Sburb* gameInstance = nullptr;

    Sburb::Sburb()
    {
        this->name = "Jterniabound";
        this->version = "1.0";
        this->icon = sf::Image();

        this->FPS = 30;
        this->FPStimeObj = sf::Clock();

        this->room = nullptr;

        if (gameInstance == nullptr) {
            gameInstance = this;
        }
    }

    Sburb::~Sburb()
    {
        AssetHandler::ClearTextures();
    }

    void Sburb::Update()
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
            if (this->shouldUpdate) {
                this->HandleAudio();
                // NOTE: Inputs were previously handled here, but are kept in the InputHandler in this port.
                this->HandleHud();

                if (this->room && !this->loadingRoom) room->Update();

                this->FocusCamera();
                this->HandleRoomChange();

                this->chooser->Update();
                this->dialoger->Update();

                this->ChainAction();
                this->UpdateWait();
            }

            // Render
            if (this->shouldDraw) {
                Render();
            }
        }
    }

    void Sburb::Render()
    {
        if (!playingMovie) {
            window->clear(sf::Color(0, 0, 0, 255));

            BatchHandler::getInstance().Reset();

            // NOTE: Some sort of translation is usually done between these renders. It may be necessary to add them back later.

            // Render all objects
            if (room) window->draw(*room);

            if (this->fade > 0.1) {
                window->draw(fadeShape);
            }

            if (dialoger) window->draw(*dialoger);

            for (auto hudElement : hud) {
                window->draw(*hudElement.second);
            }

            if (chooser) window->draw(*chooser);

            // Debugger is usually drawn here, but I don't have one.

            if (BatchHandler::getInstance().BatchExists())
                BatchHandler::getInstance().DrawBatch();

            window->display();
        }
    }

    bool Sburb::Start()
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

        // Initialize room
        if (!Serializer::LoadSerialFromXML("./levels/init.xml")) return false;

        // Start update loop
        while (window->isOpen())
        {
            Update();
        }

        return true;
    }

    // Getters
    double Sburb::GetFPS()
    {
        return this->FPS;
    }

    std::string Sburb::GetName()
    {
        return name;
    }

    std::shared_ptr<Room> Sburb::GetCurrentRoom()
    {
        return this->room;
    }

    Sburb *Sburb::GetInstance()
    {
        return gameInstance;
    }

    void Sburb::HaltUpdateProcess()
    {
        shouldUpdate = false;
        shouldDraw = false;
    }

    void Sburb::StartUpdateProcess()
    {
        shouldUpdate = true;
        shouldDraw = true;
    }
}
