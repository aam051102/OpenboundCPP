#ifndef SBURB_COMMAND_HANDLER_H
#define SBURB_COMMAND_HANDLER_H

#include "Common.h"
#include <variant>
#include "Action.h"
#include "ActionQueue.h"

namespace SBURB {
    class CommandHandler {
    public:
        static void PerformActionSilent(std::shared_ptr<Action> action, std::shared_ptr<ActionQueue> queue = nullptr);

        static void Talk(std::string info);
        static void RandomTalk(std::string info);
        static void ChangeRoom(std::string info);
        static void ChangeFocus(std::string info);
        static void Teleport(std::string info);
        static void ChangeChar(std::string info);
        static void PlaySong(std::string info);
        static void BecomeNPC(std::string info);
        static void BecomePlayer(std::string info);
        static void PlaySound(std::string info);
        static void PlayEffect(std::string info);
        static void PlayAnimation(std::string info);
        static void StartAnimation(std::string info);
        static void AddAction(std::string info);
        static void AddActions(std::string info);
        static void RemoveAction(std::string info);
        static void RemoveActions(std::string info);
        static void PresentAction(std::string info);
        static void PresentActions(std::string info);
        static void OpenChest(std::string info);
        static void DeltaSprite(std::string info);
        static void MoveSprite(std::string info);
        static void DepthSprite(std::string info);
        static void PlayMovie(std::string info);
        static void RemoveMovie(std::string info);
        static void DisableControl(std::string info);
        static void EnableControl(std::string info);
        static Trigger WaitFor(std::string info);
        static Trigger Macro(std::string info);
        static Trigger Sleep(std::string info);
        static void PauseActionQueue(std::string info);
        static void PauseActionQueues(std::string info);
        static void ResumeActionQueue(std::string info);
        static void ResumeActionQueues(std::string info);
        static void CancelActionQueue(std::string info);
        static void CancelActionQueues(std::string info);
        static void PauseActionQueueGroup(std::string info);
        static void PauseActionQueueGroups(std::string info);
        static void ResumeActionQueueGroup(std::string info);
        static void ResumeActionQueueGroups(std::string info);
        static void CancelActionQueueGroup(std::string info);
        static void CancelActionQueueGroups(std::string info);
        static void AddSprite(std::string info);
        static void RemoveSprite(std::string info);
        static void CloneSprite(std::string info);
        static void AddWalkable(std::string info);
        static void AddUnwalkable(std::string info);
        static void AddMotionPath(std::string info);
        static void RemoveWalkable(std::string info);
        static void RemoveUnwalkable(std::string info);
        static void ToggleVolume();
        static void ChangeMode(std::string info);
        static void LoadStateFile(std::string info);
        static void FadeOut();
        static void ChangeRoomRemote(std::string info);
        static void TeleportRemote(std:: string info);
        static void SetButtonState(std::string info);
        static void SkipDialog();
        static void Follow(std::string info);
        static void Unfollow(std::string info);
        static void AddOverlay(std::string info);
        static void RemoveOverlay(std::string info);
        static void Save(std::string info);
        static void Load(std::string info);
        static void SaveOrLoad(std::string info);
        static void SetGameState(std::string info);
        static void GoBack(std::string info);
        static void Try(std::string info);
        static void Walk(std::string info);
        static void OpenLink(std::string info);
        static void OpenDirect(std::string info);
        static void Cancel(std::string info);

    };
}

#endif