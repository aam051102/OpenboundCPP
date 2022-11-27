#include "CommandHandler.h"
#include "Sburb.h"
#include "Parser.h"
#include "AssetHandler.h"

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

        var lastAction;
        var newAction = lastAction = Action("waitFor", "played," + chest->GetName(), "", "");
        lastAction = lastAction.followUp = Action("waitFor", "time,13");
        lastAction = lastAction.followUp = Action("addSprite", item->GetName() + "," + Sburb::GetInstance()->GetCurrentRoom()->GetName(), "", "", nullptr, true);
        lastAction = lastAction.followUp = Action("moveSprite", item->GetName() + "," + std::to_string(chest->GetX()) + "," + std::to_string(chest->GetY() - 60), "", "", nullptr, true, true);
        lastAction = lastAction.followUp = Action("deltaSprite", item->GetName() + ",0,-3", "", "", nullptr, true, "", 10);
        
        if (AssetHandler::GetSoundByName("itemGetSound")) {
            lastAction = lastAction.followUp = Action("playSound", "itemGetSound", "", "", nullptr, true, "");
        }

        lastAction = lastAction.followUp = Action("waitFor", "time,30");
        lastAction = lastAction.followUp = Action("talk", speech);

        lastAction = lastAction.followUp = Action("removeSprite", item->GetName()  + "," + Sburb::GetInstance()->GetCurrentRoom()->GetName());
        lastAction.followUp = Sburb::GetInstance()->GetCurrentAction()->GetFollowUp();
        
        Sburb::GetInstance()->PerformAction(newAction);
    }

    void CommandHandler::DeltaSprite(std::string info)
    {
    }

    void CommandHandler::MoveSprite(std::string info)
    {
    }

    void CommandHandler::DepthSprite(std::string info)
    {
    }

    void CommandHandler::PlayMovie(std::string info)
    {
    }

    void CommandHandler::RemoveMovie(std::string info)
    {
    }

    void CommandHandler::DisableControl(std::string info) {
        Sburb.inputDisabled = trim(info).size() > 0 ? Trigger({ info }) : true;
    }

    void CommandHandler::EnableControl(std::string info)
    {
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
    }

    void CommandHandler::PauseActionQueues(std::string info)
    {
    }

    void CommandHandler::ResumeActionQueue(std::string info)
    {
    }

    void CommandHandler::ResumeActionQueues(std::string info)
    {
    }

    void CommandHandler::CancelActionQueue(std::string info)
    {
    }

    void CommandHandler::CancelActionQueues(std::string info)
    {
    }

    void CommandHandler::PauseActionQueueGroup(std::string info)
    {
    }

    void CommandHandler::PauseActionQueueGroups(std::string info)
    {
    }

    void CommandHandler::ResumeActionQueueGroup(std::string info)
    {
    }

    void CommandHandler::ResumeActionQueueGroups(std::string info)
    {
    }

    void CommandHandler::CancelActionQueueGroup(std::string info)
    {
    }

    void CommandHandler::CancelActionQueueGroups(std::string info)
    {
    }

    void CommandHandler::AddSprite(std::string info)
    {
    }

    void CommandHandler::RemoveSprite(std::string info)
    {
    }

    void CommandHandler::CloneSprite(std::string info)
    {
    }

    void CommandHandler::AddWalkable(std::string info)
    {
    }

    void CommandHandler::AddUnwalkable(std::string info)
    {
    }

    void CommandHandler::AddMotionPath(std::string info)
    {
    }

    void CommandHandler::RemoveWalkable(std::string info)
    {
    }

    void CommandHandler::RemoveUnwalkable(std::string info)
    {
    }

    void CommandHandler::ToggleVolume()
    {
    }

    void CommandHandler::ChangeMode(std::string info)
    {
    }

    void CommandHandler::LoadStateFile(std::string info)
    {
    }

    void CommandHandler::FadeOut()
    {
    }

    void CommandHandler::ChangeRoomRemote(std::string info)
    {
    }

    void CommandHandler::TeleportRemote(std::string info)
    {
    }

    void CommandHandler::SetButtonState(std::string info)
    {
    }

    void CommandHandler::SkipDialog()
    {
    }

    void CommandHandler::Follow(std::string info)
    {
    }

    void CommandHandler::Unfollow(std::string info)
    {
    }

    void CommandHandler::AddOverlay(std::string info)
    {
    }

    void CommandHandler::RemoveOverlay(std::string info)
    {
    }

    void CommandHandler::Save(std::string info)
    {
    }

    void CommandHandler::Load(std::string info)
    {
    }

    void CommandHandler::SaveOrLoad(std::string info)
    {
    }

    void CommandHandler::SetGameState(std::string info)
    {
    }

    void CommandHandler::GoBack(std::string info)
    {
    }

    void CommandHandler::Try(std::string info)
    {
    }

    void CommandHandler::Walk(std::string info)
    {
    }

    void CommandHandler::OpenLink(std::string info)
    {
    }

    void CommandHandler::OpenDirect(std::string info)
    {
    }

    void CommandHandler::Cancel(std::string info)
    {
    }
}