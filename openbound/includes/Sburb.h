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
        std::string GetName();

        void SetCurrentRoom(std::shared_ptr<Room> curRoom) { this->curRoom = curRoom; };
        std::shared_ptr<Room> GetCurrentRoom();

        void SetSprite(std::string name, std::shared_ptr<Sprite> sprite) { this->sprites[name] = sprite; };
        std::shared_ptr<Sprite> GetSprite(std::string name) { return this->sprites[name]; };

        std::map<std::string, std::shared_ptr<Room>> GetRooms() { return this->rooms; };

        void SetRoom(std::string name, std::shared_ptr<Room> room) { this->rooms[name] = room; };
        std::shared_ptr<Room> GetRoom(std::string name) { return this->rooms[name]; };

        void SetButton(std::string name, std::shared_ptr<SpriteButton> button) { this->buttons[name] = button; }
        std::shared_ptr<SpriteButton> GetButton(std::string name) { return this->buttons[name]; };

        void SetEffect(std::string, std::shared_ptr<Animation> anim) { this->effects[name] = anim; };
        std::shared_ptr<Animation> GetEffect(std::string name) { return this->effects[name]; };

        void SetGameState(std::string prop, std::string value) { this->gameState[prop] = value; };
        std::map<std::string, std::string> GetGameState() { return this->gameState; };
        std::string GetGameState(std::string prop) { return this->gameState[prop]; };

        std::map<std::string, std::shared_ptr<Sprite>> GetHud() { return this->hud; };
        std::shared_ptr<Sprite> GetHud(std::string name) { return this->hud[name]; };

        std::map<std::string, std::shared_ptr<Sprite>> GetSprites() { return this->sprites; };
        std::map<std::string, std::shared_ptr<Animation>> GetEffects() { return this->effects; };
        std::map<std::string, std::shared_ptr<SpriteButton>> GetButtons() { return this->buttons; };

        std::vector<std::shared_ptr<ActionQueue>> GetActionQueues() { return this->actionQueues; };
        std::shared_ptr<ActionQueue> GetActionQueueById(std::string id);
        void RemoveActionQueueById(std::string id);
        void RemoveActionQueuesByGroup(std::string group);
        void ForEachActionQueueInGroup(std::string group, void(*func)(std::shared_ptr<ActionQueue>));
        void AddActionQueue(std::shared_ptr<ActionQueue> queue) { this->actionQueues.push_back(queue); };

        std::shared_ptr<ActionQueue> PerformAction(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue = nullptr);

        void PerformActionInQueue(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue);

        void HandleCommandResult(std::shared_ptr<ActionQueue> queue, std::shared_ptr<Trigger> result);

        void SetMouseCursor(sf::Cursor::Type newCursor);

        void ChangeRoom(std::shared_ptr<Room> room, int newX, int newY);
        void PlayEffect(std::shared_ptr<Animation> effect, int x, int y);
        void PlaySound(std::shared_ptr<Sound> sound);
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

        float GetGlobalVolume() { return this->globalVolume; };
        void SetGlobalVolume(float globalVolume) { this->globalVolume = globalVolume; };

        void SetEngineMode(std::string engineMode) { this->engineMode = engineMode; };
        std::string GetEngineMode() { return this->engineMode; };

        void SetFading(bool fading) { this->fading = fading; };

        void SaveStateToStorage(std::string state, bool automatic, bool local);
        void LoadStateFromStorage(bool automatic, bool local);
        bool IsStateInStorage(bool automatic, bool local);
        std::string GetStateDescription(bool automatic);

        void SetNextQueueId(int nextQueueId) { this->nextQueueId = nextQueueId; };
        int GetNextQueueId() { return this->nextQueueId;};

        void SetHud(std::string name, std::shared_ptr<Sprite> sprite) { this->hud[name] = sprite; };

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

        void SetDescription(std::string description) { this->description = description; };
        std::string GetDescription() { return this->description; };

        static std::string ResolvePath(std::string path);

        void SetDimensions(float width, float height);

        Window window;

        std::string name;
        std::string levelPath;
        std::string version;
        std::string resourcePath;

    private:
        sf::Cursor::Type mouseCursor;
        std::string description;

        bool shouldUpdate;
        bool shouldDraw;

        int nextQueueId;

        std::shared_ptr<ActionQueue> queue;

        std::shared_ptr<Music> bgm;
        std::shared_ptr<Sprite> focus;
        std::shared_ptr<Sprite> destFocus;

        Vector2 camera;
        Vector2 viewSize;
        Vector2 viewPos;
        Vector2 scale;

        sf::View view;

        float globalVolume;
        std::string engineMode;

        sf::RectangleShape fadeShape;
        float fade;
        bool fading;

        int musicStoppedFor = 0;
        sf::Time lastMusicTime;

        bool playingMovie;
        bool loadingRoom;
        bool inputDisabled;

        std::shared_ptr<Trigger> inputDisabledTrigger;

        std::map<std::string, std::string> gameState;
        std::map<std::string, std::shared_ptr<Room>> rooms;
        std::map<std::string, std::shared_ptr<Sprite>> sprites;
        std::map<std::string, std::shared_ptr<SpriteButton>> buttons;
        std::map<std::string, std::shared_ptr<sf::Font>> fonts;
        std::map<std::string, std::shared_ptr<Animation>> effects;
        std::map<std::string, std::shared_ptr<Sprite>> hud;
        std::vector<std::shared_ptr<ActionQueue>> actionQueues;

        InputHandler inputHandler;

        sf::Image icon;

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
