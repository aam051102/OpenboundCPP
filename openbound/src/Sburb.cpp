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
#include "CommandHandler.h"

constexpr float FADE_RATE = 0.1;

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

        this->curRoom = nullptr;
        this->queue = std::make_shared<ActionQueue>(nullptr, "__SBURB__");

        this->view = sf::View(sf::FloatRect((float)viewPos.x, (float)viewPos.y, (float)viewSize.x, (float)viewSize.y));

        if (gameInstance == nullptr) {
            gameInstance = this;
        }
    }

    Sburb::~Sburb()
    {
        AssetHandler::ClearTextures();
    }

    void Sburb::PurgeState() {
        this->rooms = {};

        this->sprites = {};

        if (this->bgm) {
            this->bgm->Stop();
            this->bgm = nullptr;
        }

        this->gameState = {};
        this->globalVolume = 1;
        this->hud = {};
        this->sprites = {};
        this->buttons = {};
        this->effects = {};
        this->queue->SetCurrentAction(nullptr);
        this->actionQueues = {};
        this->chooser = std::make_shared<Chooser>();
        this->dialoger = nullptr;
        this->curRoom = nullptr;
        this->character = nullptr;
        this->resourcePath = "";
        this->levelPath = "";
        this->nextQueueId = 0;
        this->pressed = {};
        this->pressedOrder = {};
        this->loadedFiles = {};
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
                this->HandleInputs();
                this->HandleHud();

                if (this->curRoom && !this->loadingRoom) curRoom->Update();

                this->FocusCamera();
                this->HandleRoomChange();

                this->chooser->Update();
                this->dialoger->Update();

                this->ChainAction();
                this->UpdateWait();
            }

            this->window->setView(this->view);

            // Render
            if (this->shouldDraw) {
                Render();
            }
        }
    }

    void Sburb::HandleInputs() {
        this->SetMouseCursor(sf::Cursor::Arrow);
        if (this->HasControl() && !this->inputDisabled) {
            this->character->HandleInputs(this->pressedOrder);
        }
        else {
            this->character->MoveNone();
        }

        // NOTE: Debugger not implemented
        // Sburb.debugger.handleInputs(Sburb.pressed);
    }

    void Sburb::HandleAudio() {
        if (this->bgm && this->bgm->GetAsset()) {
            if (this->bgm->Ended()) {
                this->bgm->Loop();
            }
            
            if (this->lastMusicTime == this->bgm->GetAsset()->getPlayingOffset()) {
                this->musicStoppedFor++;
                
                if (this->musicStoppedFor > 4) {
                    this->bgm->Pause();
                    this->bgm->Play(); // asset.play() because sometimes this condition is true on startup
                }
            }
            else {
                this->musicStoppedFor = 0;
            }

            if (this->bgm->GetAsset()->getStatus() == sf::SoundStream::Paused) {
                this->bgm->Play();
            }

            this->lastMusicTime = this->bgm->GetAsset()->getPlayingOffset();
        }
    }

    void Sburb::HandleHud() {
        for (auto obj : this->hud) {
            obj.second->Update();
        }
    }

    void Sburb::FocusCamera() {
        if (!this->destFocus) {
            if (this->focus) {
                this->camera.x = this->focus->GetX() - this->viewSize.x / 2;
                this->camera.y = this->focus->GetY() - this->viewSize.y / 2;
            }
        }
        else if (abs(this->destFocus->GetX() - this->camera.x - this->viewSize.x / 2) > 4 || abs(this->destFocus->GetY() - this->camera.y - this->viewSize.y / 2) > 4) {
            this->camera.x += (this->destFocus->GetX() - this->viewSize.x / 2 - this->camera.x) / 5;
            this->camera.y += (this->destFocus->GetY() - this->viewSize.y / 2 - this->camera.y) / 5;
        }
        else {
            this->focus = this->destFocus;
            this->destFocus = nullptr;
        }
        
        this->viewPos.x = std::max(0, std::min((int)round(this->camera.x / this->scale.x) * this->scale.x, this->curRoom->GetWidth() - this->viewSize.x));
        this->viewPos.y = std::max(0, std::min((int)round(this->camera.y / this->scale.y) * this->scale.y, this->curRoom->GetHeight() - this->viewSize.y));
  
        // Move view
        this->view.setCenter(sf::Vector2f(this->viewPos.x + this->viewSize.x / 2, this->viewPos.y + this->viewSize.y / 2));
    }

    void Sburb::HandleRoomChange() {
        if (this->destRoom || this->fading) {
            if (this->fade < 1.1) {
                this->fade = std::min(1.1f, this->fade + FADE_RATE);
            }
            else if (this->destRoom) {
                float deltaX = this->destX - this->character->GetX();
                float deltaY = this->destY - this->character->GetY();
                std::shared_ptr<Character> curSprite = this->character;
               
                while (curSprite) {
                    curSprite->SetX(curSprite->GetX() + deltaX);
                    curSprite->SetY(curSprite->GetY() + deltaY);
                    curSprite->SetFollowBuffer({});
                    curSprite = curSprite->GetFollower();
                }

                this->MoveSprite(this->character, this->curRoom, this->destRoom);
                this->curRoom->Exit();
                this->curRoom = this->destRoom;
                this->curRoom->Enter();
                this->destRoom = nullptr;
            }
            else {
                this->fading = false;
            }
        }
        else if (this->HasControl() && this->fade > 0.01) {
            this->fade = std::max(0.01f, this->fade - FADE_RATE);
            //apparently alpha 0 is buggy? - NOTE: Alpha 0 probably isn't buggy in C++, but lets keep it anyway.
        }
    }

    void Sburb::ChainAction() {
        if (this->queue->GetCurrentAction()) {
            this->ChainActionInQueue(this->queue);
        }

        for (int i = 0; i < this->actionQueues.size(); i++) {
            auto queue = this->actionQueues[i];
            
            if (!queue->GetCurrentAction()) {
                this->actionQueues.erase(this->actionQueues.begin() + i);
                i--;
                continue;
            }

            if (queue->GetPaused()) {
                if ((queue->GetTrigger() && queue->GetTrigger()->CheckCompletion())) {
                    queue->SetPaused(false);
                    queue->SetTrigger(nullptr);
                }
                else {
                    continue;
                }
            }

            this->ChainActionInQueue(queue);
        }
    }

    void Sburb::ChainActionInQueue(std::shared_ptr<ActionQueue> queue) {
        auto curAction = queue->GetCurrentAction();
        
        if (curAction->GetTimes() <= 0) {
            if (curAction->GetFollowUp()) {
                if (this->HasControl() || curAction->GetFollowUp()->GetNoWait() || queue->GetNoWait()) {
                    this->PerformAction(curAction->GetFollowUp(), queue);
                }
            }
            else {
                queue->SetCurrentAction(nullptr);
            }
        }
        else if (this->HasControl() || curAction->GetNoWait() || queue->GetNoWait()) {
            this->PerformAction(curAction, queue);
        }
    }

    void Sburb::UpdateWait() {
        if (this->queue->GetTrigger()) {
            if (this->queue->GetTrigger()->CheckCompletion()) {
                this->queue->SetTrigger(nullptr);
            }
        }
        if (this->inputDisabled && this->inputDisabledTrigger) {
            if (this->inputDisabledTrigger->CheckCompletion()) {
                this->inputDisabled = false;
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
            if (this->curRoom) window->draw(*curRoom);

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
        window.Init(name, { this->viewSize.x, this->viewSize.y }, sf::Style::Close | sf::Style::Titlebar, icon); // Standard

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
        return this->curRoom;
    }

    Sburb *Sburb::GetInstance()
    {
        return gameInstance;
    }

    void Sburb::SaveStateToStorage(std::string state, bool automatic, bool local)
    {
        // TODO: NOT SUPPORTED YET
    }

    void Sburb::LoadStateFromStorage(bool automatic, bool local)
    {
        // TODO: NOT SUPPORTED YET
    }

    bool Sburb::IsStateInStorage(bool automatic, bool local)
    {
        // TODO: NOT SUPPORTED YET
        return false;
    }

    std::string Sburb::GetStateDescription(bool automatic)
    {
        // TODO: NOT SUPPORTED YET
        return "";
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

    bool Sburb::HasControl()
    {
        return !this->dialoger->GetTalking()
            && !this->chooser->GetChoosing()
            && !this->destRoom
            && !this->fading
            && !this->destFocus;
    }

    std::shared_ptr<ActionQueue> Sburb::GetActionQueueById(std::string id)
    {
        for (int i = 0; i < this->actionQueues.size(); i++) {
            auto queue = this->actionQueues[i];
            if (queue->GetId() == id) {
                return queue;
            }
        }
    }

    void Sburb::RemoveActionQueueById(std::string id)
    {
        for (int i = 0; i < this->actionQueues.size(); i++) {
            auto queue = this->actionQueues[i];
            if (queue->GetId() == id) {
                this->actionQueues.erase(this->actionQueues.begin() + i);
                return;
            }
        }
    }

    void Sburb::RemoveActionQueuesByGroup(std::string group)
    {
        for (int i = 0; i < this->actionQueues.size(); i++) {
            auto queue = this->actionQueues[i];
            if (queue->HasGroup(group)) {
                this->actionQueues.erase(this->actionQueues.begin() + i);
                i--;
            }
        }
    }

    void Sburb::ForEachActionQueueInGroup(std::string group, void(*func)(std::shared_ptr<ActionQueue>))
    {
        for (int i = 0; i < this->actionQueues.size(); i++) {
            auto queue = this->actionQueues[i];
            if (queue->HasGroup(group)) {
                func(queue);
            }
        }
    }

    std::shared_ptr<ActionQueue> Sburb::PerformAction(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue)
    {
        if (action->GetSilent()) {
            if ((action->GetTimes() == 1) && (!action->GetFollowUp())) {
                CommandHandler::PerformActionSilent(action);
                return nullptr;
            }

            if ((!queue) || (queue == this->queue)) {
                if (action->GetSilentCause() == "true") {
                    queue = std::make_shared<ActionQueue>(action);
                }
                else {
                    auto options = split(action->GetSilentCause(), ":");
                    bool noWait = (options[0] == "full") ? true : false;
                    std::string id = "";

                    if (noWait) {
                        options.erase(options.begin() + 0);
                    }

                    if (options.size() > 0) {
                        id = options[0];
                        options.erase(options.begin() + 0);
                    }

                    queue = std::make_shared<ActionQueue>(action, id, options, noWait);
                }

                this->actionQueues.push_back(queue);
            }
        }

        if (queue && (queue != this->queue)) {
            this->PerformActionInQueue(action, queue);
            return queue;
        }

        if (((this->queue->GetCurrentAction() && this->queue->GetCurrentAction()->GetFollowUp() != action && this->queue->GetCurrentAction() != action) || !this->HasControl()) && action->GetSoft()) {
            return nullptr;
        }

        this->PerformActionInQueue(action, this->queue);
        return nullptr;
    }

    void Sburb::PerformActionInQueue(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue) {
        bool looped = false;
        queue->SetCurrentAction(std::make_shared<Action>(action->Clone()));
       
        do {
            if (looped) {
                queue->SetCurrentAction(std::make_shared<Action>(queue->GetCurrentAction()->GetFollowUp()->Clone()));
            }

            std::shared_ptr<Trigger> result = CommandHandler::PerformActionSilent(queue->GetCurrentAction());
            HandleCommandResult(queue, result);
            looped = true;
        } while (queue->GetCurrentAction() && queue->GetCurrentAction()->GetTimes() <= 0 && queue->GetCurrentAction()->GetFollowUp() && queue->GetCurrentAction()->GetFollowUp()->GetNoDelay());
    }

    void Sburb::HandleCommandResult(std::shared_ptr<ActionQueue> queue, std::shared_ptr<Trigger> result) {
        if (result) {
            if (queue != this->queue) {
                queue->SetPaused(true);
                queue->SetTrigger(result);
            }
            else {
                queue->SetTrigger(result);
            }
        }
    }

    void Sburb::SetMouseCursor(sf::Cursor::Type newCursor)
    {
        sf::Cursor cursor;
        if (cursor.loadFromSystem(newCursor)) {
            window.GetWin()->setMouseCursor(cursor);
        }
    }

    void Sburb::ChangeRoom(std::shared_ptr<Room> room, int newX, int newY)
    {
        this->destRoom = room;
        this->destX = newX;
        this->destY = newY;
    }

    void Sburb::PlayEffect(std::shared_ptr<Animation> effect, int x, int y)
    {
        this->curRoom->AddEffect(std::make_shared<Animation>(effect->Clone(x, y)));
    }

    void Sburb::PlaySound(std::shared_ptr<AssetSound> sound)
    {
        sound->Stop();
        sound->Play();
    }
    
    void Sburb::PlayMovie()
    {
        // UNSUPPORTED
        //this->queue->SetTrigger(std::make_shared<Trigger>("movie," + movie->GetName() + ",5"));
        this->playingMovie = true;
    }

    void Sburb::SetCurRoomOf(std::shared_ptr<Character> sprite)
    {
        if (!this->curRoom->Contains(sprite)) {
            for (auto room : this->rooms) {
                if (room.second->Contains(sprite)) {
                    this->ChangeRoom(room.second, this->character->GetX(), this->character->GetY());
                    return;
                }
            }
        }
    }

    void Sburb::MoveSprite(std::shared_ptr<Character> sprite, std::shared_ptr<Room> oldRoom, std::shared_ptr<Room> newRoom)
    {
        std::shared_ptr<Character> curSprite = sprite;

        while (curSprite) {
            oldRoom->RemoveSprite(curSprite);
            newRoom->AddSprite(curSprite);
            curSprite = curSprite->GetFollower();
        }
    }

    std::shared_ptr<AssetMusic> Sburb::GetBGM()
    {
        return this->bgm;
    }

    void Sburb::ChangeBGM(std::shared_ptr<AssetMusic> newSong)
    {
        if(newSong) {
            if (this->bgm) {
                if (this->bgm->GetAsset() == newSong->GetAsset() && this->bgm->GetStartLoop() == newSong->GetStartLoop()) {
                    // maybe check for some kind of restart value
                    return;
                }

                this->bgm->Stop();
            }

            this->bgm = newSong;
            this->bgm->Stop();
            this->bgm->Play();
        }
    }
}
