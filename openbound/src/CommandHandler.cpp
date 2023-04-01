#include "CommandHandler.h"
#include "Sburb.h"
#include "Parser.h"
#include "AssetManager.h"
#include "Serializer.h"

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <shellapi.h>
#endif

#undef PlaySound

namespace SBURB
{
    std::shared_ptr<Trigger> CommandHandler::PerformActionSilent(std::shared_ptr<Action> action)
    {
        action->SetTimes(action->GetTimes() - 1);

        std::wstring info = action->info;
        if (info != L"")
        {
            info = trim(info);
        }

        std::wstring command = trim(action->GetCommand());

        if (command == L"talk")
            CommandHandler::Talk(info);
        else if (command == L"randomTalk")
            CommandHandler::RandomTalk(info);
        else if (command == L"changeRoom")
            CommandHandler::ChangeRoom(info);
        else if (command == L"changeFocus")
            CommandHandler::ChangeFocus(info);
        else if (command == L"teleport")
            CommandHandler::Teleport(info);
        else if (command == L"changeChar")
            CommandHandler::ChangeChar(info);
        else if (command == L"playSong")
            CommandHandler::PlaySong(info);
        else if (command == L"becomeNPC")
            CommandHandler::BecomeNPC(info);
        else if (command == L"becomePlayer")
            CommandHandler::BecomePlayer(info);
        else if (command == L"playSound")
            CommandHandler::PlaySound(info);
        else if (command == L"playEffect")
            CommandHandler::PlayEffect(info);
        else if (command == L"playAnimation")
            CommandHandler::PlayAnimation(info);
        else if (command == L"starAnimation")
            CommandHandler::StartAnimation(info);
        else if (command == L"addAction")
            CommandHandler::AddAction(info);
        else if (command == L"addActions")
            CommandHandler::AddActions(info);
        else if (command == L"removeAction")
            CommandHandler::RemoveAction(info);
        else if (command == L"removeActions")
            CommandHandler::RemoveActions(info);
        else if (command == L"presentAction")
            CommandHandler::PresentAction(info);
        else if (command == L"presentActions")
            CommandHandler::PresentActions(info);
        else if (command == L"openChest")
            CommandHandler::OpenChest(info);
        else if (command == L"deltaSprite")
            CommandHandler::DeltaSprite(info);
        else if (command == L"moveSprite")
            CommandHandler::MoveSprite(info);
        else if (command == L"depthSprite")
            CommandHandler::DepthSprite(info);
        else if (command == L"playMovie")
            CommandHandler::PlayMovie(info);
        else if (command == L"removeMovie")
            CommandHandler::RemoveMovie(info);
        else if (command == L"disableControl")
            CommandHandler::DisableControl(info);
        else if (command == L"enableControl")
            CommandHandler::EnableControl(info);
        else if (command == L"waitFor")
            return CommandHandler::WaitFor(info);
        else if (command == L"macro")
            return CommandHandler::Macro(info);
        else if (command == L"sleep")
            return CommandHandler::Sleep(info);
        else if (command == L"pauseActionQueue")
            CommandHandler::PauseActionQueue(info);
        else if (command == L"pauseActionQueues")
            CommandHandler::PauseActionQueues(info);
        else if (command == L"resumeActionQueue")
            CommandHandler::ResumeActionQueue(info);
        else if (command == L"resumeActionQueues")
            CommandHandler::ResumeActionQueues(info);
        else if (command == L"cancelActionQueue")
            CommandHandler::CancelActionQueue(info);
        else if (command == L"cancelActionQueues")
            CommandHandler::CancelActionQueues(info);
        else if (command == L"pauseActionQueueGroup")
            CommandHandler::PauseActionQueueGroup(info);
        else if (command == L"pauseActionQueueGroups")
            CommandHandler::PauseActionQueueGroups(info);
        else if (command == L"resumeActionQueueGroup")
            CommandHandler::ResumeActionQueueGroup(info);
        else if (command == L"resumeActionQueueGroups")
            CommandHandler::ResumeActionQueueGroups(info);
        else if (command == L"cancelActionQueueGroup")
            CommandHandler::CancelActionQueueGroup(info);
        else if (command == L"cancelActionQueueGroups")
            CommandHandler::CancelActionQueueGroups(info);
        else if (command == L"addSprite")
            CommandHandler::AddSprite(info);
        else if (command == L"removeSprite")
            CommandHandler::RemoveSprite(info);
        else if (command == L"cloneSprite")
            CommandHandler::CloneSprite(info);
        else if (command == L"addWalkable")
            CommandHandler::AddWalkable(info);
        else if (command == L"addUnwalkable")
            CommandHandler::AddUnwalkable(info);
        else if (command == L"addMotionPath")
            CommandHandler::AddMotionPath(info);
        else if (command == L"removeWalkable")
            CommandHandler::RemoveWalkable(info);
        else if (command == L"removeUnwalkable")
            CommandHandler::RemoveUnwalkable(info);
        else if (command == L"toggleVolume")
            CommandHandler::ToggleVolume();
        else if (command == L"changeMode")
            CommandHandler::ChangeMode(info);
        else if (command == L"loadStateFile")
            CommandHandler::LoadStateFile(info);
        else if (command == L"fadeOut")
            CommandHandler::FadeOut();
        else if (command == L"changeRoomRemote")
            CommandHandler::ChangeRoomRemote(info);
        else if (command == L"teleportRemote")
            CommandHandler::TeleportRemote(info);
        else if (command == L"setButtonState")
            CommandHandler::SetButtonState(info);
        else if (command == L"skipDialog")
            CommandHandler::SkipDialog();
        else if (command == L"follow")
            CommandHandler::Follow(info);
        else if (command == L"unfollow")
            CommandHandler::Unfollow(info);
        else if (command == L"addOverlay")
            CommandHandler::AddOverlay(info);
        else if (command == L"removeOverlay")
            CommandHandler::RemoveOverlay(info);
        else if (command == L"save")
            CommandHandler::Save(info);
        else if (command == L"load")
            CommandHandler::Load(info);
        else if (command == L"saveOrLoad")
            CommandHandler::SaveOrLoad(info);
        else if (command == L"setGameState")
            CommandHandler::SetGameState(info);
        else if (command == L"goBack")
            CommandHandler::GoBack(info);
        else if (command == L"try")
            CommandHandler::Try(info);
        else if (command == L"walk")
            CommandHandler::Walk(info);
        else if (command == L"openLink")
            CommandHandler::OpenLink(info);
        else if (command == L"openDirect")
            CommandHandler::OpenDirect(info);
        else if (command == L"cancel")
            CommandHandler::Cancel(info);

        return nullptr;
    }

    void CommandHandler::Talk(std::wstring info)
    {
        Sburb::GetInstance()->GetDialoger()->StartDialog(info);
    }

    void CommandHandler::RandomTalk(std::wstring info)
    {
        auto dialoger = Sburb::GetInstance()->GetDialoger();
        dialoger->StartDialog(info);

        int randomNum = floor(((rand() % 100) / 100.0f) * (dialoger->GetQueue().size() + 1));
        if (randomNum)
        {
            dialoger->SetQueue({ dialoger->GetQueueItem(randomNum - 1) });
            dialoger->NextDialog();
        }
        else
        {
            dialoger->SetQueue({});
        }
    }

    std::vector<std::wstring> ParseParams(std::wstring info)
    {
        std::vector<std::wstring> params = split(info, L",");

        for (int i = 0; i < params.size(); i++)
        {
            params[i] = trim(params[i]);
        }

        return params;
    }

    void CommandHandler::ChangeRoom(std::wstring info)
    {
        auto params = ParseParams(info);
        Sburb::GetInstance()->ChangeRoom(Sburb::GetInstance()->GetRoom(params[0]), stoi(params[1]), stoi(params[2]));
        Sburb::GetInstance()->SetLoadingRoom(false);
    }

    void CommandHandler::ChangeFocus(std::wstring info)
    {
        auto params = ParseParams(info);
        if (params[0] == L"null")
        {
            Sburb::GetInstance()->SetFocus(nullptr);
            Sburb::GetInstance()->SetDestFocus(nullptr);
        }
        else
        {
            std::shared_ptr<Sprite> sprite = Parser::ParseCharacterString(params[0]);
            Sburb::GetInstance()->SetDestFocus(sprite);
        }
    }

    void CommandHandler::Teleport(std::wstring info)
    {
        auto params = ParseParams(info);

        CommandHandler::ChangeRoom(info);
        Sburb::GetInstance()->PlayEffect(Sburb::GetInstance()->GetEffect(L"teleportEffect"), Sburb::GetInstance()->GetCharacter()->GetX(), Sburb::GetInstance()->GetCharacter()->GetY());
        Sburb::GetInstance()->GetQueue()->GetCurrentAction()->SetFollowUp(std::make_shared<Action>(L"playEffect", L"teleportEffect," + params[1] + L"," + params[2], L"", L"", Sburb::GetInstance()->GetQueue()->GetCurrentAction()->GetFollowUp()));
    }

    void CommandHandler::ChangeChar(std::wstring info)
    {
        auto oldCharacter = Sburb::GetInstance()->GetCharacter();
        oldCharacter->BecomeNPC();
        oldCharacter->MoveNone();
        oldCharacter->Walk();

        Sburb::GetInstance()->SetCharacter(std::static_pointer_cast<Character>(Sburb::GetInstance()->GetSprite(info)));
        auto newCharacter = Sburb::GetInstance()->GetCharacter();

        Sburb::GetInstance()->SetDestFocus(newCharacter);
        newCharacter->BecomePlayer();
        Sburb::GetInstance()->SetCurRoomOf(newCharacter);
    }

    void CommandHandler::PlaySong(std::wstring info)
    {
        auto params = ParseParams(info);

        Sburb::GetInstance()->ChangeBGM(std::make_shared<Music>(params[0], stod(params.size() == 2 ? params[1] : L"0") * 1000));
    }

    void CommandHandler::BecomeNPC(std::wstring info)
    {
        Sburb::GetInstance()->GetCharacter()->BecomeNPC();
    }

    void CommandHandler::BecomePlayer(std::wstring info)
    {
        Sburb::GetInstance()->GetCharacter()->BecomePlayer();
    }

    void CommandHandler::PlaySound(std::wstring info)
    {
        Sburb::GetInstance()->PlaySound(trim(info));
    }

    void CommandHandler::PlayEffect(std::wstring info)
    {
        auto params = ParseParams(info);
        Sburb::GetInstance()->PlayEffect(Sburb::GetInstance()->GetEffect(params[0]), stoi(params[1]), stoi(params[2]));
    }

    void CommandHandler::PlayAnimation(std::wstring info)
    {
        auto params = ParseParams(info);
        auto sprite = Parser::ParseCharacterString(params[0]);

        sprite->StartAnimation(params[1]);
    }

    void CommandHandler::StartAnimation(std::wstring info)
    {
        CommandHandler::PlayAnimation(info);
    }

    void CommandHandler::AddAction(std::wstring info)
    {
        auto params = ParseParams(info);
        size_t firstComma = info.find(L",");
        auto sprite = Parser::ParseCharacterString(params[0]);
        std::wstring actionString = info.substr(firstComma + 1, info.size());

        std::vector<std::shared_ptr<Action>> actions = Parser::ParseActionString(actionString);

        for (int i = 0; i < actions.size(); i++)
        {
            std::shared_ptr<Action> action = actions[i];
            sprite->AddAction(action);
        }
    }

    void CommandHandler::AddActions(std::wstring info)
    {
        CommandHandler::AddAction(info);
    }

    void CommandHandler::RemoveAction(std::wstring info)
    {
        auto params = ParseParams(info);
        auto sprite = Parser::ParseCharacterString(params[0]);

        for (int i = 1; i < params.size(); i++)
        {
            sprite->RemoveAction(params[i]);
        }
    }

    void CommandHandler::RemoveActions(std::wstring info)
    {
        CommandHandler::RemoveAction(info);
    }

    void CommandHandler::PresentAction(std::wstring info)
    {
        auto actions = Parser::ParseActionString(info);
        Sburb::GetInstance()->GetChooser()->SetChoices(actions);
        Sburb::GetInstance()->GetChooser()->BeginChoosing(Sburb::GetInstance()->GetCamera().x + 20, Sburb::GetInstance()->GetCamera().y + 50);
    }

    void CommandHandler::PresentActions(std::wstring info)
    {
        CommandHandler::PresentAction(info);
    }

    void CommandHandler::OpenChest(std::wstring info)
    {
        auto params = split(info, L","); // Originally limited to 2? Could be cause for concern?

        auto chest = Sburb::GetInstance()->GetSprite(trim(params[0]));
        auto item = Sburb::GetInstance()->GetSprite(trim(params[1]));
        if (chest->GetAnimation(L"open"))
        {
            chest->StartAnimation(L"open");
            if (AssetManager::GetAudioByName(L"openSound"))
            {
                CommandHandler::PlaySound(L"openSound");
            }
        }

        chest->RemoveAction(Sburb::GetInstance()->GetQueue()->GetCurrentAction()->GetName());
        int offset = params[0].size() + params[1].size() + 2;
        std::wstring speech = trim(info.substr(offset, info.size()));
        speech = speech[0] == L'@' ? speech : L"@!" + speech;

        std::shared_ptr<Action> lastAction;
        std::shared_ptr<Action> newAction = lastAction = std::make_shared<Action>(L"waitFor", L"played," + chest->GetName(), L"", L"");

        lastAction->SetFollowUp(std::make_shared<Action>(L"waitFor", L"time,13"));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>(L"addSprite", item->GetName() + L"," + Sburb::GetInstance()->GetCurrentRoom()->GetName(), L"", L"", nullptr, true));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>(L"moveSprite", item->GetName() + L"," + std::to_wstring(chest->GetX()) + L"," + std::to_wstring(chest->GetY() - 60), L"", L"", nullptr, true, true));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>(L"deltaSprite", item->GetName() + L",0,-3", L"", L"", nullptr, true, false, 10));
        lastAction = lastAction->GetFollowUp();

        if (AssetManager::GetAudioByName(L"itemGetSound"))
        {
            lastAction->SetFollowUp(std::make_shared<Action>(L"playSound", L"itemGetSound", L"", L"", nullptr, true, false));
            lastAction = lastAction->GetFollowUp();
        }

        lastAction->SetFollowUp(std::make_shared<Action>(L"waitFor", L"time,30"));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>(L"talk", speech));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>(L"removeSprite", item->GetName() + L"," + Sburb::GetInstance()->GetCurrentRoom()->GetName()));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(Sburb::GetInstance()->GetQueue()->GetCurrentAction()->GetFollowUp());

        Sburb::GetInstance()->PerformAction(newAction);
    }

    void CommandHandler::DeltaSprite(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Sprite> sprite = nullptr;

        if (params[0] == L"char")
        {
            sprite = Sburb::GetInstance()->GetCharacter();
        }
        else
        {
            sprite = Sburb::GetInstance()->GetSprite(params[0]);
        }

        int dx = stoi(params[1]);
        int dy = stoi(params[2]);
        sprite->SetX(sprite->GetX() + dx);
        sprite->SetY(sprite->GetY() + dy);
    }

    void CommandHandler::MoveSprite(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Sprite> sprite = Parser::ParseCharacterString(params[0]);
        int newX = stoi(params[1]);
        int newY = stoi(params[2]);
        sprite->SetX(newX);
        sprite->SetY(newY);
    }

    void CommandHandler::DepthSprite(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Sprite> sprite = Parser::ParseCharacterString(params[0]);
        int depth = stoi(params[1]);
        sprite->SetDepthing(depth);
    }

    void CommandHandler::PlayMovie(std::wstring info)
    {
        // UNSUPPORTED
        auto params = ParseParams(info);
        Sburb::GetInstance()->PlayMovie(/*Sburb.assets[params[0]]*/);

        /*if (params.size()> 0) {
            var interval = setInterval(function() {
                var movie = window.document.getElementById(L"movie" + params[0]);
                if (movie && (!movie.CurrentFrame || movie.CurrentFrame() >= 4)) {
                    clearInterval(interval);
                    commands.playSong(info.substring(info.indexOf(L",") + 1, info.length));
                }
            }, 10);
        }*/
    }

    void CommandHandler::RemoveMovie(std::wstring info)
    {
        // NOT SUPPORTED
        Sburb::GetInstance()->SetPlayingMovie(false);
    }

    void CommandHandler::DisableControl(std::wstring info)
    {
        if (trim(info).size() > 0)
        {
            Sburb::GetInstance()->SetInputDisabledTrigger(std::make_shared<Trigger>(std::vector({ info })));
            Sburb::GetInstance()->SetInputDisabled(false);
        }
        else
        {
            Sburb::GetInstance()->SetInputDisabled(true);
        }
    }

    void CommandHandler::EnableControl(std::wstring info)
    {
        Sburb::GetInstance()->SetInputDisabled(false);
    }

    std::shared_ptr<Trigger> CommandHandler::Macro(std::wstring info)
    {
        std::vector<std::shared_ptr<Action>> actions = Parser::ParseActionString(info);
        std::shared_ptr<Action> action = actions[0];
        if (!action->GetSilent())
        {
            action->SetSilent(L"true");
        }

        std::shared_ptr<ActionQueue> newQueue = Sburb::GetInstance()->PerformAction(action);
        if (newQueue)
        {
            return std::make_shared<Trigger>(std::vector({ L"noActions," + newQueue->GetId() }));
        }

        return nullptr;
    }

    std::shared_ptr<Trigger> CommandHandler::WaitFor(std::wstring info)
    {
        CommandHandler::DisableControl(info);
        return CommandHandler::Sleep(info);
    }

    std::shared_ptr<Trigger> CommandHandler::Sleep(std::wstring info)
    {
        return std::make_shared<Trigger>(std::vector({ info }));
    }

    void CommandHandler::PauseActionQueue(std::wstring info)
    {
        auto params = ParseParams(info);

        for (int i = 0; i < params.size(); i++)
        {
            std::shared_ptr<ActionQueue> queue = Sburb::GetInstance()->GetActionQueueById(params[i]);

            if (queue)
            {
                queue->SetPaused(true);
            }
        }
    }

    void CommandHandler::PauseActionQueues(std::wstring info)
    {
        CommandHandler::PauseActionQueue(info);
    }

    void CommandHandler::ResumeActionQueue(std::wstring info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++)
        {
            std::shared_ptr<ActionQueue> queue = Sburb::GetInstance()->GetActionQueueById(params[i]);

            if (queue)
            {
                queue->SetPaused(false);
            }
        }
    }

    void CommandHandler::ResumeActionQueues(std::wstring info)
    {
        CommandHandler::ResumeActionQueue(info);
    }

    void CommandHandler::CancelActionQueue(std::wstring info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++)
        {
            Sburb::GetInstance()->RemoveActionQueueById(params[i]);
        }
    }

    void CommandHandler::CancelActionQueues(std::wstring info)
    {
        CommandHandler::CancelActionQueue(info);
    }

    void CommandHandler::PauseActionQueueGroup(std::wstring info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++)
        {
            Sburb::GetInstance()->ForEachActionQueueInGroup(params[i], [](std::shared_ptr<ActionQueue> queue) { queue->SetPaused(true); });
        }
    }

    void CommandHandler::PauseActionQueueGroups(std::wstring info)
    {
        CommandHandler::PauseActionQueueGroup(info);
    }

    void CommandHandler::ResumeActionQueueGroup(std::wstring info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++)
        {
            Sburb::GetInstance()->ForEachActionQueueInGroup(params[i], [](std::shared_ptr<ActionQueue> queue) { queue->SetPaused(false); });
        }
    }

    void CommandHandler::ResumeActionQueueGroups(std::wstring info)
    {
        CommandHandler::ResumeActionQueueGroup(info);
    }

    void CommandHandler::CancelActionQueueGroup(std::wstring info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++)
        {
            Sburb::GetInstance()->RemoveActionQueuesByGroup(params[i]);
        }
    }

    void CommandHandler::CancelActionQueueGroups(std::wstring info)
    {
        CommandHandler::CancelActionQueueGroup(info);
    }

    void CommandHandler::AddSprite(std::wstring info)
    {
        auto params = ParseParams(info);
        auto sprite = Sburb::GetInstance()->GetSprite(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->AddSprite(sprite);
    }

    void CommandHandler::RemoveSprite(std::wstring info)
    {
        auto params = ParseParams(info);
        auto sprite = Sburb::GetInstance()->GetSprite(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->RemoveSprite(sprite);
    }

    void CommandHandler::CloneSprite(std::wstring info)
    {
        auto params = ParseParams(info);
        auto sprite = Parser::ParseCharacterString(params[0]);
        std::wstring newName = params[1];

        sprite->Clone(newName);
    }

    void CommandHandler::AddWalkable(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<AssetPath> path = AssetManager::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->AddWalkable(path);
    }

    void CommandHandler::AddUnwalkable(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<AssetPath> path = AssetManager::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->AddUnwalkable(path);
    }

    void CommandHandler::AddMotionPath(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<AssetPath> path = AssetManager::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[7]);

        room->AddMotionPath(path,
            stof(params[1]), stof(params[2]),
            stof(params[3]), stof(params[4]),
            stof(params[5]), stof(params[6]));
    }

    void CommandHandler::RemoveWalkable(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<AssetPath> path = AssetManager::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->RemoveWalkable(path);
    }

    void CommandHandler::RemoveUnwalkable(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<AssetPath> path = AssetManager::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->RemoveUnwalkable(path);
    }

    void CommandHandler::ToggleVolume()
    {
        Sburb* game = Sburb::GetInstance();

        if (game->GetGlobalVolume() >= 100)
        {
            game->SetGlobalVolume(0);
        }
        else if (game->GetGlobalVolume() >= 66)
        {
            game->SetGlobalVolume(100);
        }
        else if (game->GetGlobalVolume() >= 33)
        {
            game->SetGlobalVolume(66);
        }
        else
        {
            game->SetGlobalVolume(33);
        }

        if (game->GetBGM())
        {
            game->GetBGM()->FixVolume();
        }
    }

    void CommandHandler::ChangeMode(std::wstring info)
    {
        Sburb::GetInstance()->SetEngineMode(trim(info));
    }

    void CommandHandler::LoadStateFile(std::wstring info)
    {
        auto params = ParseParams(info);
        auto path = params[0];
        bool keepOld = params[1] == L"true";

        Serializer::LoadSerialFromXML(path, keepOld);
    }

    void CommandHandler::FadeOut()
    {
        Sburb::GetInstance()->SetFading(true);
    }

    void CommandHandler::ChangeRoomRemote(std::wstring info)
    {
        if (Sburb::GetInstance()->GetLoadingRoom())
            return;
        Sburb::GetInstance()->SetLoadingRoom(true); // Only load one room at a time

        auto params = ParseParams(info);
        std::shared_ptr<Action> lastAction;
        std::shared_ptr<Action> newAction = lastAction = std::make_shared<Action>(L"fadeOut");

        lastAction->SetFollowUp(std::make_shared<Action>(L"loadStateFile", params[0] + L"," + L"true"));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>(L"changeRoom", params[1] + L"," + params[2] + L"," + params[3]));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(Sburb::GetInstance()->GetQueue()->GetCurrentAction()->GetFollowUp());

        Sburb::GetInstance()->PerformAction(newAction);
    }

    void CommandHandler::TeleportRemote(std::wstring info)
    {
        if (Sburb::GetInstance()->GetLoadingRoom())
            return;
        Sburb::GetInstance()->SetLoadingRoom(true); // Only load one room at a time
        CommandHandler::ChangeRoomRemote(info);

        Sburb::GetInstance()->PlayEffect(Sburb::GetInstance()->GetEffect(L"teleportEffect"), Sburb::GetInstance()->GetCharacter()->GetX(), Sburb::GetInstance()->GetCharacter()->GetY());

        auto params = ParseParams(info);
        Sburb::GetInstance()->GetQueue()->GetCurrentAction()->GetFollowUp()->GetFollowUp()->SetFollowUp(std::make_shared<Action>(L"playEffect", L"teleportEffect," + params[2] + L"," + params[3], L"", L"", Sburb::GetInstance()->GetQueue()->GetCurrentAction()->GetFollowUp()->GetFollowUp()->GetFollowUp()));
    }

    void CommandHandler::SetButtonState(std::wstring info)
    {
        auto params = ParseParams(info);
        Sburb::GetInstance()->GetButton(params[0])->SetState(params[1]);
    }

    void CommandHandler::SkipDialog()
    {
        Sburb::GetInstance()->GetDialoger()->SkipAll();
    }

    void CommandHandler::Follow(std::wstring info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Character> follower = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[0]));
        std::shared_ptr<Character> leader = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[1]));

        follower->Follow(leader);
    }

    void CommandHandler::Unfollow(std::wstring info)
    {
        auto params = ParseParams(info);
        auto follower = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[0]));
        follower->Unfollow();
    }

    void CommandHandler::AddOverlay(std::wstring info)
    {
        auto params = ParseParams(info);
        auto sprite = Sburb::GetInstance()->GetSprite(params[0]);
        sprite->SetX(Sburb::GetInstance()->GetCamera().x);
        sprite->SetY(Sburb::GetInstance()->GetCamera().y);

        Sburb::GetInstance()->GetCurrentRoom()->AddSprite(sprite);
    }

    void CommandHandler::RemoveOverlay(std::wstring info)
    {
        auto params = ParseParams(info);
        auto sprite = Sburb::GetInstance()->GetSprite(params[0]);
        Sburb::GetInstance()->GetCurrentRoom()->RemoveSprite(sprite);
    }

    void CommandHandler::Save(std::wstring info)
    {
        auto params = ParseParams(info);
        bool automatic = params.size() > 0 && params[0] == L"true";
        bool local = params.size() > 1 && params[1] == L"true";

        const auto formattedName = Sburb::GetInstance()->GetCharacter()->GetName() + L", " + Sburb::GetInstance()->GetCurrentRoom()->GetName();
        Sburb::GetInstance()->SaveStateToStorage(std::string(formattedName.begin(), formattedName.end()), automatic, local);
    }

    void CommandHandler::Load(std::wstring info)
    {
        auto params = ParseParams(info);
        bool automatic = params.size() > 0 && params[0] == L"true";
        bool local = params.size() > 1 && params[1] == L"true";

        Sburb::GetInstance()->LoadStateFromStorage(automatic, local);
    }

    void CommandHandler::SaveOrLoad(std::wstring info)
    {
        auto params = ParseParams(info);
        bool local = params.size() > 0 && params[0] == L"true";
        std::vector<std::shared_ptr<Action>> actions = {};

        if (Sburb::GetInstance()->IsStateInStorage(false, local))
        {
            actions.push_back(std::make_shared<Action>(L"load", L"false, " + local, L"Load " + Sburb::GetInstance()->GetStateDescription(false)));
        }

        if (Sburb::GetInstance()->IsStateInStorage(true, local))
        {
            actions.push_back(std::make_shared<Action>(L"load", L"true, " + local, L"Load " + Sburb::GetInstance()->GetStateDescription(true)));
        }

        actions.push_back(std::make_shared<Action>(L"save", L"false," + local, L"Save"));
        actions.push_back(std::make_shared<Action>(L"cancel", L"", L"Cancel"));

        Sburb::GetInstance()->GetChooser()->SetChoices(actions);
        Sburb::GetInstance()->GetChooser()->BeginChoosing(Sburb::GetInstance()->GetCamera().x + 20, Sburb::GetInstance()->GetCamera().y + 50);
    }

    void CommandHandler::SetGameState(std::wstring info)
    {
        auto params = ParseParams(info);

        // TODO: there should be a check to make sure the gameState key
        // doesn't contain &, <, or >

        Sburb::GetInstance()->SetGameState(params[0], params[1]);
    }

    void CommandHandler::GoBack(std::wstring info)
    {
        auto params = ParseParams(info);
        auto character = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[0]));
        character->SetX(character->GetOldX());
        character->SetY(character->GetOldY());
    }

    void CommandHandler::Try(std::wstring info)
    {
        std::vector<std::shared_ptr<Trigger>> triggers = Parser::ParseTriggerString(info);

        for (int i = 0; i < triggers.size(); i++)
        {
            std::shared_ptr<Trigger> trigger = triggers[i];
            trigger->SetDetonate(true);

            if (trigger->TryToTrigger())
            {
                return;
            }
        }
    }

    void CommandHandler::Walk(std::wstring info)
    {
        auto params = ParseParams(info);
        auto character = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[0]));
        std::wstring dir = params[1];

        if (dir == L"Up")
        {
            character->MoveUp();
        }
        else if (dir == L"Down")
        {
            character->MoveDown();
        }
        else if (dir == L"Left")
        {
            character->MoveLeft();
        }
        else if (dir == L"Right")
        {
            character->MoveRight();
        }
    }

    void CommandHandler::OpenLink(std::wstring info)
    {
        auto params = ParseParams(info);
        std::wstring url = params[0];
        std::wstring text;

        if (params.size() >= 1 && params[1] != L"")
        {
            text = params[1];
        }
        else
        {
            text = url;
        }

        std::vector<std::shared_ptr<Action>> actions = {};

        actions.push_back(std::make_shared<Action>(L"openDirect", url + L"," + text, L"Go To " + text));
        actions.push_back(std::make_shared<Action>(L"cancel", L"", L"Cancel"));

        Sburb::GetInstance()->GetChooser()->SetChoices(actions);
        Sburb::GetInstance()->GetChooser()->BeginChoosing(Sburb::GetInstance()->GetCamera().x + 200, Sburb::GetInstance()->GetCamera().y + 250);
    }

    void CommandHandler::OpenDirect(std::wstring info)
    {
        auto params = ParseParams(info);
        std::wstring url = Parser::ParseURLString(params[0]);
        std::wstring text = params[1];

#if defined(_WIN32) || defined(WIN32)
        ShellExecute(0, 0, std::wstring(url.begin(), url.end()).c_str(), 0, 0, SW_SHOW);
#endif
        // TODO: Add support for opening URL on other OS's.
    }

    void CommandHandler::Cancel(std::wstring info)
    {
    }
}