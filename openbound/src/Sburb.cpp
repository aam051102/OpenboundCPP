#include <sstream>
#include <algorithm>
#include "Sburb.h"
#include "Logger.h"
#include "Sprite.h"
#include "AssetManager.h"
#include "BatchHandler.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "AssetPath.h"
#include "Serializer.h"
#include "Parser.h"
#include "CommandHandler.h"

#include <thread>

#if defined(_WIN32) || defined(WIN32)
#include <atlstr.h>
#include <dwrite.h>
#include <shobjidl.h> 

#undef PlaySound
#endif

constexpr float FADE_RATE = 0.1;

namespace SBURB
{
    static Sburb *gameInstance = nullptr;

    Sburb::Sburb()
    {
        this->name = "Jterniabound";
        this->version = "1.0";
        this->icon = sf::Image();
        this->description = "";
        this->engineMode = "wander";

        this->FPS = 30;
        this->FPStimeObj = sf::Clock();

        this->curRoom = nullptr;
        this->globalVolume = 100;
        this->levelPath = "";
        this->resourcePath = "";
        this->queue = std::make_shared<ActionQueue>(nullptr, "__SBURB__");
        this->playingMovie = false;
        this->loadingRoom = false;
        this->inputDisabled = false;

        this->viewSize = Vector2(650, 450);
        this->view = sf::View(sf::FloatRect((float)viewPos.x, (float)viewPos.y, (float)viewSize.x, (float)viewSize.y));

        this->destRoom = nullptr;
        this->destFocus = nullptr;
        this->bgm = nullptr;
        this->camera = Vector2();

        this->buttons = {};
        this->actionQueues = {};
        this->effects = {};
        this->hud = {};
        this->sprites = {};
        this->rooms = {};
        this->fonts = {};
        this->sounds = {};

        this->character = nullptr;
        this->chooser = nullptr;
        this->destX = 0;
        this->destY = 0;
        this->dialoger = nullptr;
        this->focus = nullptr;
        this->inputDisabledTrigger = nullptr;

        this->lastMusicTime = sf::Time::Zero;
        this->fading = false;
        this->fade = 0;
        this->fadeShape = sf::RectangleShape();
        this->fadeShape.setSize(sf::Vector2f(this->viewSize.x, this->viewSize.y));
        this->musicStoppedFor = 0;
        this->nextQueueId = 0;

        this->cursors = {};
        this->LoadMouseCursor(sf::Cursor::Arrow);
        this->LoadMouseCursor(sf::Cursor::Hand);

        if (gameInstance == nullptr)
        {
            gameInstance = this;
        }
    }

    Sburb::~Sburb()
    {
        AssetManager::ClearGraphics();
        AssetManager::ClearAudio();
        AssetManager::ClearText();
        AssetManager::ClearPaths();
        AssetManager::ClearMovies();
        AssetManager::ClearFonts();
    }

    void Sburb::PurgeState()
    {
        if (this->bgm)
        {
            this->bgm->Stop();
            this->bgm = nullptr;
        }

        this->rooms.clear();
        this->gameState.clear();
        this->globalVolume = 100;
        this->hud.clear();
        this->sounds.clear();
        this->sprites.clear();
        this->buttons.clear();
        this->effects.clear();
        this->queue->SetCurrentAction(nullptr);
        this->actionQueues.clear();
        this->chooser = std::make_shared<Chooser>();
        this->dialoger = nullptr;
        this->curRoom = nullptr;
        this->character = nullptr;
        this->resourcePath = "";
        this->levelPath = "";
        this->nextQueueId = 0;

        for (auto cursor : this->cursors) {
            cursor.second.reset();
        }

        this->cursors.clear();

        this->LoadMouseCursor(sf::Cursor::Arrow);
        this->LoadMouseCursor(sf::Cursor::Hand);
    }

    void Sburb::BeginChoosing()
    {
        this->character->Idle();
        this->chooser->BeginChoosing(this->character->GetX(), this->character->GetY());
    }

    void Sburb::Update()
    {
        sf::Int32 FPStime = FPStimeObj.getElapsedTime().asMilliseconds();

        // Event polling
        sf::Event event;
        while (window->pollEvent(event))
        {
            inputHandler.Update(event, window->hasFocus());

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
            if (this->shouldUpdate)
            {
                this->mouseCursor = sf::Cursor::Arrow;

                this->HandleAudio();
                this->HandleInputs();
                this->HandleHud();

                if (this->curRoom && !this->loadingRoom)
                {
                    curRoom->Update();
                }

                this->FocusCamera();
                this->HandleRoomChange();

                if (this->chooser)
                    this->chooser->Update();
                if (this->dialoger)
                    this->dialoger->Update();

                this->ChainAction();
                this->UpdateWait();

                // Update mouse cursor
                window.GetWin()->setMouseCursor(*cursors[this->mouseCursor]);
            }

            this->window->setView(this->view);

            // Render
            if (this->shouldDraw)
            {
                Render();
            }
            else
            {
                RenderPreloader();
            }
        }
    }

    void Sburb::HandleInputs()
    {
        if (this->character)
        {
            if (this->HasControl() && !this->inputDisabled)
            {
                this->character->HandleInputs(InputHandler::GetPressedOrder());
            }
            else
            {
                this->character->MoveNone();
            }
        }

        // NOTE: Debugger not implemented
        // Sburb.debugger.handleInputs(Sburb.pressed);
    }

    void Sburb::HandleAudio()
    {
        return;
        if (this->bgm && this->bgm->GetAsset())
        {
            if (this->bgm->Ended())
            {
                this->bgm->Loop();
            }

            if (this->lastMusicTime == this->bgm->GetAsset()->getPlayingOffset())
            {
                this->musicStoppedFor++;

                if (this->musicStoppedFor > 4)
                {
                    this->bgm->Pause();
                    this->bgm->Play(); // asset.play() because sometimes this condition is true on startup
                }
            }
            else
            {
                this->musicStoppedFor = 0;
            }

            if (this->bgm->GetAsset()->getStatus() == sf::SoundStream::Paused)
            {
                this->bgm->Play();
            }

            this->lastMusicTime = this->bgm->GetAsset()->getPlayingOffset();
        }
    }

    void Sburb::HandleHud()
    {
        for (auto obj : this->hud)
        {
            obj.second->Update();
        }
    }

    void Sburb::FocusCamera()
    {
        if (!this->destFocus)
        {
            if (this->focus)
            {
                this->camera.x = this->focus->GetX() - this->viewSize.x / 2;
                this->camera.y = this->focus->GetY() - this->viewSize.y / 2;
            }
        }
        else if (abs(this->destFocus->GetX() - this->camera.x - this->viewSize.x / 2) > 4 || abs(this->destFocus->GetY() - this->camera.y - this->viewSize.y / 2) > 4)
        {
            this->camera.x += (this->destFocus->GetX() - this->viewSize.x / 2 - this->camera.x) / 5;
            this->camera.y += (this->destFocus->GetY() - this->viewSize.y / 2 - this->camera.y) / 5;
        }
        else
        {
            this->focus = this->destFocus;
            this->destFocus = nullptr;
        }

        if (this->curRoom)
        {
            this->viewPos.x = std::max(0, std::min((int)round(this->camera.x / this->scale.x) * this->scale.x, this->curRoom->GetWidth() - this->viewSize.x));
            this->viewPos.y = std::max(0, std::min((int)round(this->camera.y / this->scale.y) * this->scale.y, this->curRoom->GetHeight() - this->viewSize.y));
        }

        // Move view
        this->view.setCenter(sf::Vector2f(this->viewPos.x + this->viewSize.x / 2, this->viewPos.y + this->viewSize.y / 2));
    }

    void Sburb::HandleRoomChange()
    {
        if (this->destRoom || this->fading)
        {
            if (this->fade < 1.1)
            {
                this->fade = std::min(1.1f, this->fade + FADE_RATE);
            }
            else if (this->destRoom)
            {
                float deltaX = this->destX - this->character->GetX();
                float deltaY = this->destY - this->character->GetY();
                std::shared_ptr<Character> curSprite = this->character;

                while (curSprite)
                {
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
            else
            {
                this->fading = false;
            }
        }
        else if (this->HasControl() && this->fade > 0.01)
        {
            this->fade = std::max(0.01f, this->fade - FADE_RATE);
            // apparently alpha 0 is buggy? - NOTE: Alpha 0 probably isn't buggy in C++, but lets keep it anyway.
        }

        if (this->fade > 0.01)
        {
            float limitedFade = this->fade;
            if (this->fade > 1)
                limitedFade = 1;
            if (this->fade < 0)
                limitedFade = 0;

            this->fadeShape.setFillColor(sf::Color(0, 0, 0, 255 / 1 * limitedFade));
        }
    }

    void Sburb::ChainAction()
    {
        if (this->queue->GetCurrentAction())
        {
            this->ChainActionInQueue(this->queue);
        }

        for (int i = 0; i < this->actionQueues.size(); i++)
        {
            auto queue = this->actionQueues[i];

            if (!queue->GetCurrentAction())
            {
                this->actionQueues.erase(this->actionQueues.begin() + i);
                i--;
                continue;
            }

            if (queue->GetPaused())
            {
                if ((queue->GetTrigger() && queue->GetTrigger()->CheckCompletion()))
                {
                    queue->SetPaused(false);
                    queue->SetTrigger(nullptr);
                }
                else
                {
                    continue;
                }
            }

            this->ChainActionInQueue(queue);
        }
    }

    void Sburb::ChainActionInQueue(std::shared_ptr<ActionQueue> queue)
    {
        auto curAction = queue->GetCurrentAction();

        if (curAction->GetTimes() <= 0)
        {
            if (curAction->GetFollowUp())
            {
                if (this->HasControl() || curAction->GetFollowUp()->GetNoWait() || queue->GetNoWait())
                {
                    this->PerformAction(curAction->GetFollowUp(), queue);
                }
            }
            else
            {
                queue->SetCurrentAction(nullptr);
            }
        }
        else if (this->HasControl() || curAction->GetNoWait() || queue->GetNoWait())
        {
            this->PerformAction(curAction, queue);
        }
    }

    void Sburb::UpdateWait()
    {
        if (this->queue->GetTrigger())
        {
            if (this->queue->GetTrigger()->CheckCompletion())
            {
                this->queue->SetTrigger(nullptr);
            }
        }
        if (this->inputDisabled && this->inputDisabledTrigger)
        {
            if (this->inputDisabledTrigger->CheckCompletion())
            {
                this->inputDisabled = false;
            }
        }
    }

    void Sburb::RenderPreloader()
    {
        window->clear(sf::Color(0, 0, 0, 255));

        auto preloaderBG = AssetManager::GetGraphicByName("preloaderBG");

        if (preloaderBG)
        {
            sf::Sprite preloaderBGSprite;
            preloaderBGSprite.setPosition(0, 0);
            preloaderBGSprite.setTexture(*preloaderBG->GetAsset());
            window->draw(preloaderBGSprite);
        }

        // Check if Verdana is defined - use SburbFont otherwise
        auto font = AssetManager::GetFontByName("Verdana");
        if (!font)
            font = AssetManager::GetFontByName("SburbFont");

        if (font)
        {
            int percent = floor((AssetManager::GetTotalLoaded() / (float)AssetManager::GetTotalAssets()) * 100);

            sf::Text textWriter;
            textWriter.setString(std::to_string(percent) + "%");
            textWriter.setColor(sf::Color::White);
            textWriter.setFont(*font->GetAsset());
            textWriter.setCharacterSize(10);
            textWriter.setStyle(font->GetStyle());
            textWriter.setOrigin(sf::Vector2f(textWriter.getLocalBounds().width / 2.0f, 0));
            textWriter.setPosition(this->viewSize.x / 2, this->viewSize.y - 50);

            window->draw(textWriter);
        }

        window->display();

        /*if (this->error.length) {
            Sburb.stage.textAlign = "left";
            for (var i = 0; i < this.error.length; i++)
                Sburb.stage.fillText(
                    "Error: " + this.error[i],
                    10,
                    20 + 15 * i
                );
            Sburb.stage.textAlign = "center";
            if (this.failed.length) {
                if (this.refresh) {
                    this.error = ["Refreshing..."];
                    for (var i = 0; i < this.failed.length; i++)
                        this.assets[this.failed[i]].reload();
                    this.failed = [];
                }
                else {
                    Sburb.stage.font = "18px Verdana";
                    Sburb.stage.fillText(
                        "Press SPACE to reload failed assets",
                        Sburb.Stage.width / 2,
                        Sburb.Stage.height - 70
                    );
                }
            }
        }*/
    }

    void Sburb::Render()
    {
        if (!this->playingMovie)
        {
            window->clear(sf::Color(0, 0, 0, 255));

            BatchHandler::getInstance().Reset();

            // Render all objects
            if (this->curRoom)
                window->draw(*curRoom);

            if (BatchHandler::getInstance().BatchExists())
                BatchHandler::getInstance().DrawBatch();

            // The following elements are rendered relative to the current view position. (i.e. screen space / fixed on screen)
            sf::RenderStates viewState;
            viewState.transform.translate(sf::Vector2f(this->viewPos.x, this->viewPos.y));

            if (this->fade > 0.1)
            {
                window->draw(fadeShape, viewState);
            }

            if (dialoger)
                window->draw(*dialoger, viewState);

            for (auto hudElement : hud)
            {
                window->draw(*hudElement.second, viewState);
            }

            // From here, things are rendered relative to 0, 0. (i.e. world space)
            if (chooser)
                window->draw(*chooser);

            if (BatchHandler::getInstance().BatchExists())
                BatchHandler::getInstance().DrawBatch();

            // Debugger is usually drawn here, but I don't have one.

            window->display();
        }
    }

#if defined(_WIN32) || defined(WIN32)
    // This is the worst function I have ever seen and it comes directly from the official documentation.
    // This is how Microsoft wants us to open dialogs.
    // *throws up*
    std::string OpenDialog(HWND hwnd) {
        const COMDLG_FILTERSPEC c_rgSaveTypes[] =
        {
            {L"SburbML File (*.xml)",        L"*.xml"},
            {L"All Documents (*.*)",         L"*.*"}
        };

        #define INDEX_SBURBML 1

        std::string filePath = "";

        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr))
        {
            IFileOpenDialog* pFileOpen;

            // Create the FileOpenDialog object.
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

            if (SUCCEEDED(hr))
            {
                hr = pFileOpen->SetTitle(L"Select initialization file");
                if (SUCCEEDED(hr))
                {
                    // Set the file types to display only. 
                    // Notice that this is a 1-based array.
                    hr = pFileOpen->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
                    if (SUCCEEDED(hr))
                    {
                        // Set the selected file type index
                        hr = pFileOpen->SetFileTypeIndex(INDEX_SBURBML);
                        if (SUCCEEDED(hr))
                        {
                            // Set the default extension
                            hr = pFileOpen->SetDefaultExtension(L"xml");
                            if (SUCCEEDED(hr))
                            {
                                // Show the Open dialog box.
                                hr = pFileOpen->Show(NULL);

                                // Get the file name from the dialog box.
                                if (SUCCEEDED(hr))
                                {
                                    IShellItem* pItem;
                                    hr = pFileOpen->GetResult(&pItem);
                                    if (SUCCEEDED(hr))
                                    {
                                        PWSTR pszFilePath;
                                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                                        // Display the file name to the user.
                                        if (SUCCEEDED(hr))
                                        {
                                            filePath = CW2A(pszFilePath);
                                            CoTaskMemFree(pszFilePath);
                                        }
                                        pItem->Release();
                                    }
                                }
                                pFileOpen->Release();
                            }
                        }
                    }
                }
            }
            CoUninitialize();
        }

        return filePath;
    }
#endif

    bool Sburb::Start()
    {
        // Create & initialize main window
        window.Init(name, {this->viewSize.x, this->viewSize.y}, sf::Style::Close | sf::Style::Titlebar, icon); // Standard
        
        if (!window.GetWin())
        {
            GlobalLogger->Log(Logger::Error, "Failed to create main game window.");
            return false;
        }

        // Load and set icon
#if defined(_WIN32) || defined(WIN32)
        HRSRC hIconInfo = FindResourceEx(nullptr, RT_ICON, MAKEINTRESOURCE(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));

        if (hIconInfo != NULL) {
            HGLOBAL hMemory = LoadResource(nullptr, hIconInfo);

            if (hMemory != NULL) {
                DWORD hMemorySize = SizeofResource(nullptr, hIconInfo);

                if (hMemorySize != NULL) {
                    char* cBuffer = new char[hMemorySize];
                    cBuffer = (char*)LockResource(hMemory);

                    if (cBuffer != NULL) {
                        sf::Image icon;
                        if (icon.loadFromMemory(cBuffer, hMemorySize))
                        {
                            window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
                        }
                        else {
                            GlobalLogger->Log(Logger::Error, "Failed to load icon.");
                        }
                    }
                }
            }
        }
#endif

        // Center window
        window.CenterWindow();

        std::string initFilePath = "";

#if defined(_WIN32) || defined(WIN32)
        initFilePath = OpenDialog(window->getSystemHandle());
#endif

        // Fallback path
        if (initFilePath == "") initFilePath = "levels/init.xml";

        // Initialize room
        std::thread t1(Serializer::LoadSerialFromXML, initFilePath, false);

        // Start update loop
        while (window->isOpen())
        {
            Update();
        }

        t1.join();

        return true;
    }

    // Getters
    double Sburb::GetFPS()
    {
        return this->FPS;
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
        if (!this->dialoger || !this->chooser)
            return false;
        return !this->dialoger->GetTalking() && !this->chooser->GetChoosing() && !this->destRoom && !this->fading && !this->destFocus;
    }

    std::string Sburb::ResolvePath(std::string path)
    {
        // NOTE: Used to have ?version=[version] added
        if (path.find(gameInstance->resourcePath) == std::string::npos)
        {
            return gameInstance->resourcePath + "/" + path;
        }

        return path;
    }

    void Sburb::SetDimensions(float width, float height)
    {
        this->window->setSize(sf::Vector2u(width, height));
        this->viewSize = Vector2(width, height);
        this->fadeShape.setSize(sf::Vector2f(this->viewSize.x, this->viewSize.y));
    }

    void Sburb::SetName(std::string name) {
        this->name = name;
        this->window->setTitle(name);
    };
    
    std::shared_ptr<ActionQueue> Sburb::GetActionQueueById(std::string id)
    {
        for (int i = 0; i < this->actionQueues.size(); i++)
        {
            auto queue = this->actionQueues[i];
            if (queue->GetId() == id)
            {
                return queue;
            }
        }

        return nullptr;
    }

    void Sburb::RemoveActionQueueById(std::string id)
    {
        for (int i = 0; i < this->actionQueues.size(); i++)
        {
            auto queue = this->actionQueues[i];
            if (queue->GetId() == id)
            {
                this->actionQueues.erase(this->actionQueues.begin() + i);
                return;
            }
        }
    }

    void Sburb::RemoveActionQueuesByGroup(std::string group)
    {
        for (int i = 0; i < this->actionQueues.size(); i++)
        {
            auto queue = this->actionQueues[i];
            if (queue->HasGroup(group))
            {
                this->actionQueues.erase(this->actionQueues.begin() + i);
                i--;
            }
        }
    }

    void Sburb::ForEachActionQueueInGroup(std::string group, void (*func)(std::shared_ptr<ActionQueue>))
    {
        for (int i = 0; i < this->actionQueues.size(); i++)
        {
            auto queue = this->actionQueues[i];
            if (queue->HasGroup(group))
            {
                func(queue);
            }
        }
    }

    std::shared_ptr<ActionQueue> Sburb::PerformAction(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue)
    {
        if (action->GetSilent())
        {
            if ((action->GetTimes() == 1) && (!action->GetFollowUp()))
            {
                CommandHandler::PerformActionSilent(action);
                return nullptr;
            }

            if ((!queue) || (queue == this->queue))
            {
                if (action->GetSilentCause() == "true")
                {
                    queue = std::make_shared<ActionQueue>(action);
                }
                else
                {
                    auto options = split(action->GetSilentCause(), ":");
                    bool noWait = (options[0] == "full") ? true : false;
                    std::string id = "";

                    if (noWait)
                    {
                        options.erase(options.begin() + 0);
                    }

                    if (options.size() > 0)
                    {
                        id = options[0];
                        options.erase(options.begin() + 0);
                    }

                    queue = std::make_shared<ActionQueue>(action, id, options, noWait);
                }

                this->actionQueues.push_back(queue);
            }
        }

        if (queue && (queue != this->queue))
        {
            this->PerformActionInQueue(action, queue);
            return queue;
        }

        if (((this->queue->GetCurrentAction() &&
              this->queue->GetCurrentAction()->GetFollowUp() != action &&
              this->queue->GetCurrentAction() != action) ||
             !this->HasControl()) &&
            action->GetSoft())
        {
            return nullptr;
        }

        this->PerformActionInQueue(action, this->queue);
        return nullptr;
    }

    void Sburb::PerformActionInQueue(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue)
    {
        bool looped = false;
        queue->SetCurrentAction(action->Clone());

        do
        {
            if (looped)
            {
                queue->SetCurrentAction(queue->GetCurrentAction()->GetFollowUp()->Clone());
            }

            std::shared_ptr<Trigger> result = CommandHandler::PerformActionSilent(queue->GetCurrentAction());
            HandleCommandResult(queue, result);
            looped = true;
        } while (queue->GetCurrentAction() &&
                 queue->GetCurrentAction()->GetTimes() <= 0 &&
                 queue->GetCurrentAction()->GetFollowUp() &&
                 queue->GetCurrentAction()->GetFollowUp()->GetNoDelay());
    }

    void Sburb::HandleCommandResult(std::shared_ptr<ActionQueue> queue, std::shared_ptr<Trigger> result)
    {
        if (result)
        {
            if (queue != this->queue)
            {
                queue->SetPaused(true);
                queue->SetTrigger(result);
            }
            else
            {
                queue->SetTrigger(result);
            }
        }
    }

    void Sburb::SetMouseCursor(sf::Cursor::Type newCursor)
    {
        this->mouseCursor = newCursor;

        this->LoadMouseCursor(newCursor);
    }

    void Sburb::LoadMouseCursor(sf::Cursor::Type newCursor) {
        if (cursors.find(newCursor) == cursors.end()) {
            std::shared_ptr<sf::Cursor> cursor = std::make_shared<sf::Cursor>();

            if (cursor->loadFromSystem(newCursor)) {
                this->cursors.insert(std::pair(newCursor, cursor));
            }
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
        this->curRoom->AddEffect(effect->Clone(x, y));
    }

    void Sburb::PlaySound(std::string name)
    {
        if (!this->sounds[name])
        {
            this->sounds[name] = std::make_shared<Sound>(name, AssetManager::GetAudioByName(name));
        }

        auto sound = this->sounds[name];
        sound->Stop();
        sound->Play();
    }

    void Sburb::PlayMovie()
    {
        // UNSUPPORTED
        // this->queue->SetTrigger(std::make_shared<Trigger>("movie," + movie->GetName() + ",5"));
        this->playingMovie = true;
    }

    void Sburb::SetCurRoomOf(std::shared_ptr<Character> sprite)
    {
        if (!this->curRoom->Contains(sprite))
        {
            for (auto room : this->rooms)
            {
                if (room.second->Contains(sprite))
                {
                    this->ChangeRoom(room.second, this->character->GetX(), this->character->GetY());
                    return;
                }
            }
        }
    }

    void Sburb::MoveSprite(std::shared_ptr<Character> sprite, std::shared_ptr<Room> oldRoom, std::shared_ptr<Room> newRoom)
    {
        std::shared_ptr<Character> curSprite = sprite;

        while (curSprite)
        {
            oldRoom->RemoveSprite(curSprite);
            newRoom->AddSprite(curSprite);
            curSprite = curSprite->GetFollower();
        }
    }

    std::shared_ptr<Music> Sburb::GetBGM()
    {
        return this->bgm;
    }

    void Sburb::ChangeBGM(std::shared_ptr<Music> newSong)
    {
        if (newSong)
        {
            if (this->bgm)
            {
                if (this->bgm->GetAsset() == newSong->GetAsset() && this->bgm->GetStartLoop() == newSong->GetStartLoop())
                {
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
