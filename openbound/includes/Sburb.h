#ifndef SBURB_GAME_H
#define SBURB_GAME_H

#include "Common.h"
#include "Window.h"
#include "Input.h"
#include "Room.h"
#include "AssetManager.h"
#include "Character.h"
#include "Dialoger.h"
#include "SpriteButton.h"

#include <pugixml.hpp>

namespace SBURB
{
    // Handles a running instance of the game.
    class Sburb
    {
    public:
        Sburb();
        ~Sburb();
        static Sburb *GetInstance();

        void Update();
        void Render();

        bool Start();

        double GetFPS();
        std::string GetName();
        Room* GetRoom();

        void AddSprite(std::string name, std::shared_ptr<Sprite> sprite);

        std::shared_ptr<Sprite> GetSprite(std::string name) { return this->sprites[name]; };
        std::shared_ptr<SpriteButton> GetButton(std::string name) { return this->buttons[name]; };
        std::shared_ptr<sf::Font> GetFont(std::string name) { return this->fonts[name]; };
        std::string GetGameState(std::string prop) { return this->gameState[prop]; };

        void PerformAction(std::shared_ptr<Action> action);

        void SetMouseCursor(std::string cursor);

        std::shared_ptr<Character> GetCharacter() { return this->character; };
        std::shared_ptr<Dialoger> GetDialoger() { return this->dialoger; };

        Window window;
        AssetManager assetManager;

        std::string name;
        std::string levelPath;
        std::string version;
        std::string resourcePath;

    private:
        std::map<std::string, std::string> gameState;
        std::map<std::string, std::shared_ptr<Sprite>> sprites;
        std::map<std::string, std::shared_ptr<SpriteButton>> buttons;
        std::map<std::string, std::shared_ptr<sf::Font>> fonts;

        InputHandler inputHandler;

        sf::Image icon;

        sf::Int32 FPS;
        sf::Clock FPStimeObj;

        Room *room;
        std::shared_ptr<Character> character;
        std::shared_ptr<Dialoger> dialoger;
    };
}

#endif
