#include "CommandHandler.h"
#include "Sburb.h"
#include "Parser.h"
#include "AssetHandler.h"
#include "Serializer.h"

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <shellapi.h>
#include <windows.h>
#endif

namespace SBURB {
    CommandHandler::CommandHandler() {
        
    }

    void CommandHandler::PerformActionSilent(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue)
    {
        action->SetTimes(action->GetTimes() - 1);

        std::string info = action->info;
        if (info != "") {
            info = trim(info);
        }

        std::string command = trim(action->GetCommand());

    }

    void CommandHandler::Talk(std::string info)
    {
        Sburb::GetInstance()->GetDialoger()->StartDialog(info);
    }

    void CommandHandler::RandomTalk(std::string info)
    {
        auto dialoger = Sburb::GetInstance()->GetDialoger();
        dialoger->StartDialog(info);

        int randomNum = floor(rand() * (dialoger->GetQueue().size() + 1));
        if (randomNum) {
            dialoger->SetQueue({ dialoger->GetQueueItem(randomNum - 1) });
            dialoger->NextDialog();
        }
        else {
            dialoger->SetQueue({});
        }
    }

    std::vector<std::string> ParseParams(std::string info) {
        std::vector<std::string> params = split(info, ",");
        
        for (int i = 0; i < params.size(); i++) {
            params[i] = trim(params[i]);
        }

        return params;
    }

    void CommandHandler::ChangeRoom(std::string info)
    {
        auto params = ParseParams(info);
        Sburb::GetInstance()->ChangeRoom(Sburb::GetInstance()->GetRoom(params[0]), stoi(params[1]), stoi(params[2]));
        Sburb::GetInstance()->SetLoadingRoom(false);
    }

    void CommandHandler::ChangeFocus(std::string info)
    {
        auto params = ParseParams(info);
        if (params[0] == "null") {
            Sburb::GetInstance()->SetFocus(nullptr);
            Sburb::GetInstance()->SetDestFocus(nullptr);
        }
        else {
            std::shared_ptr<Sprite> sprite = Parser::ParseCharacterString(params[0]);
            Sburb::GetInstance()->SetDestFocus(sprite);
        }
    }

    void CommandHandler::Teleport(std::string info)
    {
        auto params = ParseParams(info);

        CommandHandler::ChangeRoom(info);
        Sburb::GetInstance()->PlayEffect(Sburb::GetInstance()->GetEffect("teleportEffect"), Sburb::GetInstance()->GetCharacter()->GetX(), Sburb::GetInstance()->GetCharacter()->GetY());
        Sburb::GetInstance()->GetCurrentAction()->SetFollowUp(Action("playEffect", "teleportEffect," + params[1] + "," + params[2], "", "", Sburb::GetInstance()->GetCurrentAction()->GetFollowUp()));
    }

    void CommandHandler::ChangeChar(std::string info)
    {
        auto oldCharacter = Sburb::GetInstance()->GetCharacter();
        oldCharacter->BecomeNPC();
        oldCharacter->MoveNone();
        oldCharacter->Walk();

        Sburb::GetInstance()->SetCharacter(Sburb::GetInstance()->GetSprite(info));
        auto newCharacter = Sburb::GetInstance()->GetCharacter();

        Sburb::GetInstance()->SetDestFocus(newCharacter);
        newCharacter->BecomePlayer();
        Sburb::GetInstance()->SetCurRoomOf(newCharacter);
    }

    void CommandHandler::PlaySong(std::string info)
    {
        auto params = ParseParams(info);

        Sburb::GetInstance()->ChangeBGM(BGM(AssetHandler::GetSoundByName(params[0]), stof(params[1])));
    }

    void CommandHandler::BecomeNPC(std::string info)
    {
        Sburb::GetInstance()->GetCharacter()->BecomeNPC();
    }

    void CommandHandler::BecomePlayer(std::string info)
    {
        Sburb::GetInstance()->GetCharacter()->BecomePlayer();
    }

    void CommandHandler::PlaySound(std::string info)
    {
        Sburb::GetInstance()->PlaySound(Sound(AssetHandler::GetSoundByName(trim(info))));
    }

    void CommandHandler::PlayEffect(std::string info)
    {
        auto params = ParseParams(info);
        Sburb::GetInstance()->PlayEffect(Sburb::GetInstance()->GetEffect(params[0]), stoi(params[1]), stoi(params[2]));
    }

    void CommandHandler::PlayAnimation(std::string info)
    {
        auto params = ParseParams(info);
        auto sprite = Parser::ParseCharacterString(params[0]);

        sprite->StartAnimation(params[1]);
    }

    void CommandHandler::StartAnimation(std::string info)
    {
        CommandHandler::PlayAnimation(info);
    }

    void CommandHandler::AddAction(std::string info)
    {
        auto params = ParseParams(info);
        int firstComma = info.find(",");
        auto sprite = Parser::ParseCharacterString(params[0]);
        std::string actionString = info.substr(firstComma + 1, info.size());

        std::vector<Action> actions = ParseActionString(actionString);

        for (int i = 0; i < actions.size(); i++) {
            Action action = actions[i];
            sprite->AddAction(std::make_shared<Action>(action));
        }
    }

    void CommandHandler::AddActions(std::string info)
    {
        CommandHandler::AddAction(info);
    }

    void CommandHandler::RemoveAction(std::string info)
    {
        auto params = ParseParams(info);
        auto sprite = Parser::ParseCharacterString(params[0]);

        for (int i = 1; i < params.size(); i++) {
            sprite->RemoveAction(params[i]);
        }
    }

    void CommandHandler::RemoveActions(std::string info)
    {
        CommandHandler::RemoveAction(info);
    }

    void CommandHandler::PresentAction(std::string info)
    {
        auto actions = ParseActionString(info);
        Sburb::GetInstance()->GetChooser()->SetChoices(actions);
        Sburb::GetInstance()->GetChooser()->BeginChoosing(Sburb::GetInstance()->GetCamera()->GetX() + 20, Sburb::GetInstance()->GetCamera()->GetY() + 50);
    }

    void CommandHandler::PresentActions(std::string info)
    {
        CommandHandler::PresentAction(info);
    }

    void CommandHandler::OpenChest(std::string info)
    {
        auto params = split(info, ","); // Originally limited to 2? Could be cause for concern?

        auto chest = Sburb::GetInstance()->GetSprite(trim(params[0]));
        auto item = Sburb::GetInstance()->GetSprite(trim(params[1]));
        if (chest->GetAnimation("open")) {
            chest->StartAnimation("open");
            if (AssetHandler::GetSoundByName("openSound")) {
                CommandHandler::PlaySound("openSound");
            }
        }

        chest->RemoveAction(Sburb::GetInstance()->GetCurrentAction()->GetName());
        int offset = params[0].size() + params[1].size() + 2;
        std::string speech = trim(info.substr(offset, info.size()));
        speech = speech[0] == '@' ? speech : "@!" + speech;

        std::shared_ptr<Action> lastAction;
        std::shared_ptr<Action> newAction = lastAction = std::make_shared<Action>("waitFor", "played," + chest->GetName(), "", "");

        lastAction->SetFollowUp(std::make_shared<Action>("waitFor", "time,13"));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>("addSprite", item->GetName() + "," + Sburb::GetInstance()->GetCurrentRoom()->GetName(), "", "", nullptr, true));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>("moveSprite", item->GetName() + "," + std::to_string(chest->GetX()) + "," + std::to_string(chest->GetY() - 60), "", "", nullptr, true, true));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>("deltaSprite", item->GetName() + ",0,-3", "", "", nullptr, true, "", 10));
        lastAction = lastAction->GetFollowUp();
        
        if (AssetHandler::GetSoundByName("itemGetSound")) {
            lastAction->SetFollowUp(std::make_shared<Action>("playSound", "itemGetSound", "", "", nullptr, true, ""));
            lastAction = lastAction->GetFollowUp();
        }

        lastAction->SetFollowUp(std::make_shared<Action>("waitFor", "time,30"));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>("talk", speech));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>("removeSprite", item->GetName()  + "," + Sburb::GetInstance()->GetCurrentRoom()->GetName()));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(Sburb::GetInstance()->GetCurrentAction()->GetFollowUp());
        
        Sburb::GetInstance()->PerformAction(newAction);
    }

    void CommandHandler::DeltaSprite(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Sprite> sprite = nullptr;

        if (params[0] == "char") {
            sprite = Sburb::GetInstance()->GetCharacter();
        }
        else {
            sprite = Sburb::GetInstance()->GetSprite(params[0]);
        }

        int dx = stoi(params[1]);
        int dy = stoi(params[2]);
        sprite->SetX(sprite->GetX() + dx);
        sprite->SetY(sprite->GetY() + dy);
    }

    void CommandHandler::MoveSprite(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Sprite> sprite = Parser::ParseCharacterString(params[0]);
        int newX = stoi(params[1]);
        int newY = stoi(params[2]);
        sprite->SetX(newX);
        sprite->SetY(newY);
    }

    void CommandHandler::DepthSprite(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Sprite> sprite = Parser::ParseCharacterString(params[0]);
        int depth = stoi(params[1]);
        sprite->SetDepthing(depth);
    }

    void CommandHandler::PlayMovie(std::string info)
    {
        // NOT SUPPORTED
        auto params = ParseParams(info);
        Sburb::GetInstance()->PlayMovie(/*Sburb.assets[params[0]]*/);

        /*if (params.size()> 0) {
            var interval = setInterval(function() {
                var movie = window.document.getElementById("movie" + params[0]);
                if (movie && (!movie.CurrentFrame || movie.CurrentFrame() >= 4)) {
                    clearInterval(interval);
                    commands.playSong(info.substring(info.indexOf(",") + 1, info.length));
                }
            }, 10);
        }*/
    }

    void CommandHandler::RemoveMovie(std::string info)
    {
        // NOT SUPPORTED
        Sburb::GetInstance()->SetPlayingMovie(false);
    }

    void CommandHandler::DisableControl(std::string info) {
        Sburb::GetInstance()->SetInputDisabled(trim(info).size() > 0 ? Trigger({ info }) : true);
    }

    void CommandHandler::EnableControl(std::string info)
    {
        Sburb::GetInstance()->SetInputDisabled(false);
    }

    Trigger CommandHandler::Macro(std::string info) {
        std::vector<Action> actions = ParseActionString(info);
        Action action = actions[0];
        if (!action.GetIsSilent()) {
            action.SetIsSilent(true);
        }

        std::shared_ptr<ActionQueue> newQueue = Sburb::GetInstance()->PerformAction(std::make_shared<Action>(action));
        if (newQueue) {
            return Trigger({ "noActions," + newQueue->GetId() });
        }
    }

    Trigger CommandHandler::WaitFor(std::string info) {
        CommandHandler::DisableControl(info);
        return CommandHandler::Sleep(info);
    }

    Trigger CommandHandler::Sleep(std::string info) {
        return Trigger({ info });
    }

    void CommandHandler::PauseActionQueue(std::string info)
    {
        auto params = ParseParams(info);

        for (int i = 0; i < params.size(); i++) {
            std::shared_ptr<ActionQueue> queue = Sburb::GetInstance()->GetActionQueueById(params[i]);
            
            if (queue) {
                queue->SetPaused(true);
            }
        }
    }

    void CommandHandler::PauseActionQueues(std::string info)
    {
        CommandHandler::PauseActionQueue(info);
    }

    void CommandHandler::ResumeActionQueue(std::string info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++) {
            std::shared_ptr<ActionQueue> queue = Sburb::GetInstance()->GetActionQueueById(params[i]);

            if (queue) {
                queue->SetPaused(false);
            }
        }
    }

    void CommandHandler::ResumeActionQueues(std::string info)
    {
        CommandHandler::ResumeActionQueue(info);
    }

    void CommandHandler::CancelActionQueue(std::string info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++) {
            Sburb::GetInstance()->RemoveActionQueueById(params[i]);
        }
    }

    void CommandHandler::CancelActionQueues(std::string info)
    {
        CommandHandler::CancelActionQueue(info);
    }

    void CommandHandler::PauseActionQueueGroup(std::string info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++) {
            Sburb::GetInstance()->ForEachActionQueueInGroup(params[i], [](std::shared_ptr<ActionQueue> queue) {
                queue->SetPaused(true);
            });
        }
    }

    void CommandHandler::PauseActionQueueGroups(std::string info)
    {
        CommandHandler::PauseActionQueueGroup(info);
    }

    void CommandHandler::ResumeActionQueueGroup(std::string info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.size(); i++) {
            Sburb::GetInstance()->ForEachActionQueueInGroup(params[i], [](std::shared_ptr<ActionQueue> queue) {
                queue->SetPaused(false);
            });
        }
    }

    void CommandHandler::ResumeActionQueueGroups(std::string info)
    {
        CommandHandler::ResumeActionQueueGroup(info);
    }

    void CommandHandler::CancelActionQueueGroup(std::string info)
    {
        auto params = ParseParams(info);
        for (int i = 0; i < params.length; i++) {
            Sburb::GetInstance()->RemoveActionQueuesByGroup(params[i]);
        }
    }

    void CommandHandler::CancelActionQueueGroups(std::string info)
    {
        CommandHandler::CancelActionQueueGroup(info);
    }

    void CommandHandler::AddSprite(std::string info)
    {
        auto params = ParseParams(info);
        auto sprite = Sburb::GetInstance()->GetSprite(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->AddSprite(sprite);
    }

    void CommandHandler::RemoveSprite(std::string info)
    {
        auto params = ParseParams(info);
        auto sprite = Sburb::GetInstance()->GetSprite(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->RemoveSprite(sprite);
    }

    void CommandHandler::CloneSprite(std::string info)
    {
        auto params = ParseParams(info);
        auto sprite = Parser::ParseCharacterString(params[0]);
        std::string newName = params[1];

        sprite->Clone(newName);
    }

    void CommandHandler::AddWalkable(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Path> path = AssetHandler::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->AddWalkable(path);
    }

    void CommandHandler::AddUnwalkable(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Path> path = AssetHandler::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->AddUnwalkable(path);
    }

    void CommandHandler::AddMotionPath(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Path> path = AssetHandler::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[7]);

        room->AddMotionPath(path,
            stof(params[1]), stof(params[2]),
            stof(params[3]), stof(params[4]),
            stof(params[5]), stof(params[6]));
    }

    void CommandHandler::RemoveWalkable(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Path> path = AssetHandler::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->RemoveWalkable(path);
    }

    void CommandHandler::RemoveUnwalkable(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Path> path = AssetHandler::GetPathByName(params[0]);
        auto room = Sburb::GetInstance()->GetRoom(params[1]);

        room->RemoveUnwalkable(path);
    }

    void CommandHandler::ToggleVolume()
    {
        Sburb* game = Sburb::GetInstance();

        if (game->GetGlobalVolume() >= 1) {
            game->SetGlobalVolume(0);
        }
        else if (game->GetGlobalVolume() >= 0.6) {
            game->SetGlobalVolume(1);
        }
        else if (game->GetGlobalVolume() >= 0.3) {
            game->SetGlobalVolume(0.66);
        }
        else {
            game->SetGlobalVolume(0.33);
        }

        if (game->GetBGM()) {
            game->GetBGM()->FixVolume();
        }
    }

    void CommandHandler::ChangeMode(std::string info)
    {
        Sburb::GetInstance()->SetEngineMode(trim(info));
    }

    void CommandHandler::LoadStateFile(std::string info)
    {
        auto params = ParseParams(info);
        auto path = params[0];
        bool keepOld = params[1] == "true";

        Serializer::LoadSerialFromXML(path, keepOld);
    }

    void CommandHandler::FadeOut()
    {
        Sburb::GetInstance()->SetFading(true);
    }

    void CommandHandler::ChangeRoomRemote(std::string info)
    {
        if (Sburb::GetInstance()->GetLoadingRoom()) return;
        Sburb::GetInstance()->SetLoadingRoom(true); //Only load one room at a time

        auto params = ParseParams(info);
        std::shared_ptr<Action> lastAction;
        std::shared_ptr<Action> newAction = lastAction = std::make_shared<Action>("fadeOut");

        lastAction->SetFollowUp(std::make_shared<Action>("loadStateFile", params[0] + "," + "true"));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(std::make_shared<Action>("changeRoom", params[1] + "," + params[2] + "," + params[3]));
        lastAction = lastAction->GetFollowUp();

        lastAction->SetFollowUp(Sburb::GetInstance()->GetCurrentAction()->GetFollowUp());
        
        Sburb::GetInstance()->PerformAction(newAction);
    }

    void CommandHandler::TeleportRemote(std::string info)
    {
        if (Sburb::GetInstance()->GetLoadingRoom()) return;
        Sburb::GetInstance()->SetLoadingRoom(true); //Only load one room at a time
        CommandHandler::ChangeRoomRemote(info);

        Sburb::GetInstance()->PlayEffect(Sburb::GetEffect("teleportEffect"), Sburb::GetInstance()->GetCharacter()->GetX(), Sburb::GetInstance()->GetCharacter()->GetY());

        auto params = ParseParams(info);
        Sburb::GetInstance()->GetCurrentAction()->GetFollowUp()->GetFollowUp()->SetFollowUp(std::make_shared<Action>("playEffect", "teleportEffect," + params[2] + "," + params[3], "", "", Sburb::GetInstance()->GetCurrentAction()->GetFollowUp()->GetFollowUp()->GetFollowUp());
    }

    void CommandHandler::SetButtonState(std::string info)
    {
        auto params = ParseParams(info);
        Sburb::GetInstance()->GetButton(params[0])->SetState(params[1]);
    }

    void CommandHandler::SkipDialog()
    {
        Sburb::GetInstance()->GetDialoger()->SkipAll();
    }

    void CommandHandler::Follow(std::string info)
    {
        auto params = ParseParams(info);
        std::shared_ptr<Character> follower = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[0]));
        std::shared_ptr<Character> leader = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[1]));

        follower->Follow(leader);
    }

    void CommandHandler::Unfollow(std::string info)
    {
        auto params = ParseParams(info);
        auto follower = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[0]));
        follower->Unfollow();
    }

    void CommandHandler::AddOverlay(std::string info)
    {
        auto params = ParseParams(info);
        auto sprite = Sburb::GetInstance()->GetSprite(params[0]);
        sprite->SetX(Sburb::GetInstance()->GetCamera()->GetX());
        sprite->SetY(Sburb::GetInstance()->GetCamera()->GetX());

        Sburb::GetInstance()->GetCurrentRoom()->AddSprite(sprite);
    }

    void CommandHandler::RemoveOverlay(std::string info)
    {
        auto params = ParseParams(info);
        auto sprite = Sburb::GetInstance()->GetSprite(params[0]);
        Sburb::GetInstance()->GetCurrentRoom()->RemoveSprite(sprite);
    }

    void CommandHandler::Save(std::string info)
    {
        auto params = ParseParams(info);
        bool automatic = params.size() > 0 && params[0] == "true";
        bool local = params.size() > 1 && params[1] == "true";

        Sburb::GetInstance()->SaveStateToStorage(Sburb::GetInstance()->GetCharacter()->GetName() + ", " + Sburb::GetInstance()->GetRoom()->GetName(), automatic, local);
    }

    void CommandHandler::Load(std::string info)
    {
        auto params = ParseParams(info);
        bool automatic = params.size() > 0 && params[0] == "true";
        bool local = params.size() > 1 && params[1] == "true";

        Sburb::GetInstance()->LoadStateFromStorage(automatic, local);
    }

    void CommandHandler::SaveOrLoad(std::string info)
    {
        auto params = ParseParams(info);
        bool local = params.size() > 0 && params[0] == "true";
        std::vector<Action> actions = {};

        if (Sburb::GetInstance()->IsStateInStorage(false, local)) {
            actions.push_back(Action("load", "false, " + local, "Load " + Sburb::GetInstance()->GetStateDescription(false)));
        }

        if (Sburb::GetInstance()->IsStateInStorage(true, local)) {
            actions.push_back(Action("load", "true, " + local, "Load " + Sburb::GetInstance()->GetStateDescription(true)));
        }

        if (Sburb::GetInstance()->tests.storage) {
            actions.push_back(Action("save", "false," + local, "Save"));
        }

        actions.push_back(Action("cancel", "", "Cancel"));

        Sburb::GetInstance()->GetChooser()->SetChoices(actions);
        Sburb::GetInstance()->GetChooser()->BeginChoosing(Sburb::GetInstance()->GetCamera()->GetX() + 20, Sburb::GetInstance()->GetCamera()->GetY() + 50);
    }

    void CommandHandler::SetGameState(std::string info)
    {
        auto params = ParseParams(info);

        // TODO: there should be a check to make sure the gameState key
        // doesn't contain &, <, or >

        Sburb::GetInstance()->SetGameState(params[0], params[1]);
    }

    void CommandHandler::GoBack(std::string info)
    {
        auto params = ParseParams(info);
        auto character = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[0]));
        character->SetX(character->GetOldX());
        character->SetY(character->GetOldY());
    }

    void CommandHandler::Try(std::string info)
    {
        std::vector<Trigger> triggers = Parser::ParseTriggerString(info);
        
        for (int i = 0; i < triggers.size(); i++) {
            Trigger trigger = triggers[i];
            trigger.SetDetonate(true);

            if (trigger.TryToTrigger()) {
                return;
            }
        }
    }

    void CommandHandler::Walk(std::string info)
    {
        auto params = ParseParams(info);
        auto character = std::static_pointer_cast<Character>(Parser::ParseCharacterString(params[0]));
        std::string dir = params[1];

        if (dir == "Up") {
            character->MoveUp();
        } else if (dir == "Down") {
            character->MoveDown();
        } else if (dir == "Left") {
            character->MoveLeft();
        } else if (dir == "Right") {
            character->MoveRight();
        }
    }

    void CommandHandler::OpenLink(std::string info)
    {
        auto params = ParseParams(info);
        std::string url = params[0];
        std::string text;
        
        if (params.size() >= 1 && params[1] != "") {
            text = params[1];
        }
        else {
            text = url;
        }

        std::vector<Action> actions = {};

        actions.push_back(Action("openDirect", url + "," + text, "Go To " + text));
        actions.push_back(Action("cancel", "", "Cancel"));

        Sburb::GetInstance()->GetChooser()->SetChoices(actions);
        Sburb::GetInstance()->GetChooser()->BeginChoosing(Sburb::GetInstance()->GetCamera()->GetX() + 200, Sburb::GetInstance()->GetCamera()->GetY() + 250);
    }

    void CommandHandler::OpenDirect(std::string info)
    {
        auto params = ParseParams(info);
        std::string url = Parser::ParseURLstring(params[0]);
        std::string text = params[1];

#if defined(_WIN32) || defined(WIN32)
        ShellExecute(0, 0, std::wstring(url.begin(), url.end()).c_str(), 0, 0, SW_SHOW);
#endif
        // TODO: Add support for opening on other OS's.
    }

    void CommandHandler::Cancel(std::string info)
    {
    }
}