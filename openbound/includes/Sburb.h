#ifndef SBURB_GAME_H
#define SBURB_GAME_H

#include "Common.h"
#include "Window.h"
#include "Input.h"
#include "Room.h"
#include "Character.h"
#include "SpriteButton.h"
#include "Chooser.h"
#include "Music.h"
#include "Sound.h"
#include "ActionQueue.h"
#include "Dialoger.h"

#include <SFML/Window/Cursor.hpp>

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
        void RenderPreloader();

        void PurgeState();

        void HandleInputs();
        void HandleAudio();
        void HandleHud();
        void FocusCamera();
        void HandleRoomChange();
        void ChainAction();
        void UpdateWait();
        void BeginChoosing();

        void ChainActionInQueue(std::shared_ptr<ActionQueue> queue);

        bool Start();

        double GetFPS();

        void SetCurrentRoom(std::shared_ptr<Room> curRoom) { this->curRoom = curRoom; };
        std::shared_ptr<Room> GetCurrentRoom();

        void SetSprite(std::wstring name, std::shared_ptr<Sprite> sprite) { this->sprites[name] = sprite; };
        std::shared_ptr<Sprite> GetSprite(std::wstring name) { return this->sprites[name]; };

        std::map<std::wstring, std::shared_ptr<Room>> GetRooms() { return this->rooms; };

        void SetRoom(std::wstring name, std::shared_ptr<Room> room) { this->rooms[name] = room; };
        std::shared_ptr<Room> GetRoom(std::wstring name) { return this->rooms[name]; };

        void SetButton(std::wstring name, std::shared_ptr<SpriteButton> button) { this->buttons[name] = button; }
        std::shared_ptr<SpriteButton> GetButton(std::wstring name) { return this->buttons[name]; };

        void SetEffect(std::wstring, std::shared_ptr<Animation> anim) { this->effects[name] = anim; };
        std::shared_ptr<Animation> GetEffect(std::wstring name) { return this->effects[name]; };

        void SetGameState(std::wstring prop, std::wstring value) { this->gameState[prop] = value; };
        std::map<std::wstring, std::wstring> GetGameState() { return this->gameState; };
        std::wstring GetGameState(std::wstring prop) { return this->gameState[prop]; };

        std::map<std::wstring, std::shared_ptr<Sprite>> GetHud() { return this->hud; };
        std::shared_ptr<Sprite> GetHud(std::wstring name) { return this->hud[name]; };

        std::map<std::wstring, std::shared_ptr<Sprite>> GetSprites() { return this->sprites; };
        std::map<std::wstring, std::shared_ptr<Animation>> GetEffects() { return this->effects; };
        std::map<std::wstring, std::shared_ptr<SpriteButton>> GetButtons() { return this->buttons; };

        std::vector<std::shared_ptr<ActionQueue>> GetActionQueues() { return this->actionQueues; };
        std::shared_ptr<ActionQueue> GetActionQueueById(std::wstring id);
        void RemoveActionQueueById(std::wstring id);
        void RemoveActionQueuesByGroup(std::wstring group);
        void ForEachActionQueueInGroup(std::wstring group, void(*func)(std::shared_ptr<ActionQueue>));
        void AddActionQueue(std::shared_ptr<ActionQueue> queue) { this->actionQueues.push_back(queue); };

        std::shared_ptr<ActionQueue> PerformAction(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue = nullptr);

        void PerformActionInQueue(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue);

        void HandleCommandResult(std::shared_ptr<ActionQueue> queue, std::shared_ptr<Trigger> result);

        void SetMouseCursor(sf::Cursor::Type newCursor);
        void LoadMouseCursor(sf::Cursor::Type newCursor);

        void ChangeRoom(std::shared_ptr<Room> room, int newX, int newY);
        void PlayEffect(std::shared_ptr<Animation> effect, int x, int y);
        void PlaySound(std::wstring name);
        void PlayMovie();
        void SetCurRoomOf(std::shared_ptr<Character> sprite);
        void MoveSprite(std::shared_ptr<Character> sprite, std::shared_ptr<Room> oldRoom, std::shared_ptr<Room> newRoom);

        std::shared_ptr<Music> GetBGM();
        void ChangeBGM(std::shared_ptr<Music> music);

        void SetPlayingMovie(bool playingMovie) { this->playingMovie = playingMovie; };
        void SetInputDisabled(bool inputDisabled) { this->inputDisabled = inputDisabled; };
        void SetInputDisabledTrigger(std::shared_ptr<Trigger> trigger) { this->inputDisabledTrigger = trigger; };
        void SetLoadingRoom(bool loadingRoom) { this->loadingRoom = loadingRoom; };
        bool GetLoadingRoom() { return this->loadingRoom; };

        void SetCharacter(std::shared_ptr<Character> character) { this->character = character; };
        std::shared_ptr<Character> GetCharacter() { return this->character; };

        std::shared_ptr<Chooser> GetChooser() { return this->chooser; };

        void SetDialoger(std::shared_ptr<Dialoger> dialoger) { this->dialoger = dialoger; };
        std::shared_ptr<Dialoger> GetDialoger() { return this->dialoger; };

        void SetFocus(std::shared_ptr<Sprite> focus) { this->focus = focus; };

        void SetDestFocus(std::shared_ptr<Sprite> destFocus) { this->destFocus = destFocus; };

        int GetGlobalVolume() { return this->globalVolume; };
        void SetGlobalVolume(int globalVolume) { this->globalVolume = globalVolume; };

        void SetEngineMode(std::wstring engineMode) { this->engineMode = engineMode; };
        std::wstring GetEngineMode() { return this->engineMode; };

        void SetFading(bool fading) { this->fading = fading; };

        void SaveStateToStorage(std::string state = "SaveFile", bool automatic = false, bool local = false);
        void LoadStateFromStorage(bool automatic = false, bool local = false);
        bool IsStateInStorage(bool automatic, bool local);
        std::wstring GetStateDescription(bool automatic = false, bool local = false);
        void DeleteStateFromStorage(bool automatic = false, bool local = false);
        void DeleteOldVersionStates(bool local = false);

        void SetNextQueueId(int nextQueueId) { this->nextQueueId = nextQueueId; };
        int GetNextQueueId() { return this->nextQueueId;};

        void SetHud(std::wstring name, std::shared_ptr<Sprite> sprite) { this->hud[name] = sprite; };

        void HaltUpdateProcess();
        void StartUpdateProcess();

        bool HasControl();

        std::shared_ptr<ActionQueue> GetQueue() { return this->queue; };

        void SetCamera(Vector2 camera) { this->camera = camera; };
        Vector2 GetCamera() { return this->camera; };

        void SetScale(Vector2 scale) { this->scale = scale; };
        Vector2 GetScale() { return this->scale; };

        Vector2 GetViewPos() { return this->viewPos; };
        Vector2 GetViewSize() { return this->viewSize; };

        bool GetShouldUpdate() { return this->shouldUpdate; };

        bool GetInputDisabled() { return this->inputDisabled; };

        void SetDescription(std::wstring description) { this->description = description; };
        std::wstring GetDescription() { return this->description; };

        static std::wstring ResolvePath(std::wstring path);

        void SetDimensions(float width, float height);

        std::wstring GetName() { return this->name; };
        void SetName(std::wstring name);

        Window window;

        std::wstring levelPath;
        std::wstring version;
        std::wstring resourcePath;

    private:
        std::wstring name;

        sf::Cursor::Type mouseCursor;
        std::wstring description;

        bool shouldUpdate;
        bool shouldDraw;

        int nextQueueId;

        std::shared_ptr<ActionQueue> queue;

        std::shared_ptr<Music> bgm;
        std::shared_ptr<Sprite> focus;
        std::shared_ptr<Sprite> destFocus;

        std::unordered_map<sf::Cursor::Type, std::shared_ptr<sf::Cursor>> cursors;

        Vector2 camera;
        Vector2 viewSize;
        Vector2 viewPos;
        Vector2 scale;

        sf::View view;

        int globalVolume;
        std::wstring engineMode;

        sf::RectangleShape fadeShape;
        float fade;
        bool fading;

        int musicStoppedFor = 0;
        sf::Time lastMusicTime;

        bool playingMovie;
        bool loadingRoom;
        bool inputDisabled;

        std::shared_ptr<Trigger> inputDisabledTrigger;

        std::map<std::wstring, std::wstring> gameState;
        std::map<std::wstring, std::shared_ptr<Room>> rooms;
        std::map<std::wstring, std::shared_ptr<Sprite>> sprites;
        std::map<std::wstring, std::shared_ptr<Sound>> sounds;
        std::map<std::wstring, std::shared_ptr<SpriteButton>> buttons;
        std::map<std::wstring, std::shared_ptr<sf::Font>> fonts;
        std::map<std::wstring, std::shared_ptr<Animation>> effects;
        std::map<std::wstring, std::shared_ptr<Sprite>> hud;
        std::vector<std::shared_ptr<ActionQueue>> actionQueues;

        InputHandler inputHandler;

        bool isFullscreen;
        sf::Int32 FPS;
        sf::Clock FPStimeObj;

        int destX;
        int destY;

        std::shared_ptr<Room> destRoom;
        std::shared_ptr<Room> curRoom;
        std::shared_ptr<Character> character;
        std::shared_ptr<Dialoger> dialoger;
        std::shared_ptr<Chooser> chooser;
    };
}

#endif
