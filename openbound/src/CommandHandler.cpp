#include "CommandHandler.h"
#include "Game.h"

namespace SBURB {
    CommandHandler::CommandHandler() {
        this->commands = {
            {"talk", (void*)[](std::string info) {
                //Game::GetInstance()->GetDialoger()->StartDialog(info);
            }}/*,
            {"randomTalk", [](std::string info) {
                auto dialoger = Game::GetInstance()->GetDialoger();
                dialoger->StartDialog(info);

                int randomNum = floor(rand() * (dialoger->GetQueue().size() + 1));
                if (randomNum) {
                    Sburb.dialoger.queue = [dialoger->GetQueue()[randomNum - 1]];


                    dialoger->NextDialog();
                }
                else {
                    dialoger->ClearQueue();
                }
            }}*/
        };
    }
}