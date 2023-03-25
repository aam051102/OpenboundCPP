#ifndef SBURB_COMMAND_HANDLER_H
#define SBURB_COMMAND_HANDLER_H

#include "Common.h"
#include <variant>
#include "Action.h"
#include "ActionQueue.h"

namespace SBURB {
    class CommandHandler {
    public:
        static std::shared_ptr<Trigger> PerformActionSilent(std::shared_ptr<Action> action);

        static void Talk(std::wstring info);
        static void RandomTalk(std::wstring info);
        static void ChangeRoom(std::wstring info);
        static void ChangeFocus(std::wstring info);
        static void Teleport(std::wstring info);
        static void ChangeChar(std::wstring info);
        static void PlaySong(std::wstring info);
        static void BecomeNPC(std::wstring info);
        static void BecomePlayer(std::wstring info);
        static void PlaySound(std::wstring info);
        static void PlayEffect(std::wstring info);
        static void PlayAnimation(std::wstring info);
        static void StartAnimation(std::wstring info);
        static void AddAction(std::wstring info);
        static void AddActions(std::wstring info);
        static void RemoveAction(std::wstring info);
        static void RemoveActions(std::wstring info);
        static void PresentAction(std::wstring info);
        static void PresentActions(std::wstring info);
        static void OpenChest(std::wstring info);
        static void DeltaSprite(std::wstring info);
        static void MoveSprite(std::wstring info);
        static void DepthSprite(std::wstring info);
        static void PlayMovie(std::wstring info);
        static void RemoveMovie(std::wstring info);
        static void DisableControl(std::wstring info);
        static void EnableControl(std::wstring info);
        /*
        DEPRECATED: DO NOT USE
        */
        static std::shared_ptr<Trigger> WaitFor(std::wstring info);
        static std::shared_ptr<Trigger> Macro(std::wstring info);
        static std::shared_ptr<Trigger> Sleep(std::wstring info);
        static void PauseActionQueue(std::wstring info);
        static void PauseActionQueues(std::wstring info);
        static void ResumeActionQueue(std::wstring info);
        static void ResumeActionQueues(std::wstring info);
        static void CancelActionQueue(std::wstring info);
        static void CancelActionQueues(std::wstring info);
        static void PauseActionQueueGroup(std::wstring info);
        static void PauseActionQueueGroups(std::wstring info);
        static void ResumeActionQueueGroup(std::wstring info);
        static void ResumeActionQueueGroups(std::wstring info);
        static void CancelActionQueueGroup(std::wstring info);
        static void CancelActionQueueGroups(std::wstring info);
        static void AddSprite(std::wstring info);
        static void RemoveSprite(std::wstring info);
        static void CloneSprite(std::wstring info);
        static void AddWalkable(std::wstring info);
        static void AddUnwalkable(std::wstring info);
        static void AddMotionPath(std::wstring info);
        static void RemoveWalkable(std::wstring info);
        static void RemoveUnwalkable(std::wstring info);
        static void ToggleVolume();
        static void ChangeMode(std::wstring info);
        static void LoadStateFile(std::wstring info);
        static void FadeOut();
        static void ChangeRoomRemote(std::wstring info);
        static void TeleportRemote(std::wstring info);
        static void SetButtonState(std::wstring info);
        static void SkipDialog();
        static void Follow(std::wstring info);
        static void Unfollow(std::wstring info);
        static void AddOverlay(std::wstring info);
        static void RemoveOverlay(std::wstring info);
        static void Save(std::wstring info);
        static void Load(std::wstring info);
        static void SaveOrLoad(std::wstring info);
        static void SetGameState(std::wstring info);
        static void GoBack(std::wstring info);
        static void Try(std::wstring info);
        static void Walk(std::wstring info);
        static void OpenLink(std::wstring info);
        static void OpenDirect(std::wstring info);
        static void Cancel(std::wstring info);

    };
}

#endif