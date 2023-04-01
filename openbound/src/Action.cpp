#include "Action.h"

namespace SBURB {
    Action::Action(std::wstring command, std::wstring info, std::wstring name, std::wstring sprite, std::shared_ptr<Action> followUp, bool noWait, bool noDelay, uint16_t times, bool soft, std::wstring silent) {
        this->command = command;
        this->info = info;

        this->silentCause = silent;

        if (silent == L"" || silent == L"false") {
            this->silent = false;
        }
        else {
            this->silent = true;
        }

        this->soft = soft;
        this->name = name;
        this->noWait = noWait;
        this->noDelay = noDelay;
        this->followUp = followUp;
        this->sprite = sprite;
        this->times = times;
    }

    Action::~Action() {

    }

    std::shared_ptr<Action> Action::Clone() {
        return std::make_shared<Action>(this->command, this->info, this->name, this->sprite, this->followUp, this->noWait, this->noDelay, this->times, this->soft, this->silentCause);
    }

    std::wstring Action::Serialize(std::wstring output) {
        std::wstring newOutput = output + L"\n<action " +
            L"command='" + this->command +
            (this->sprite != L"" ? L"' sprite='" + this->sprite : L"") +
            (this->name != L"" ? L"' name='" + this->name : L"") +
            (this->noWait ? L"' noWait='" + this->noWait : L"") +
            (this->noDelay ? L"' noDelay='" + this->noDelay : L"") +
            (this->soft ? L"' soft='" + this->soft : L"") +
            (this->silentCause != L"" ? L"' silent='" + this->silentCause : L"") +
            (this->times != 1 ? L"' times='" + this->times : L"") +
            L"'>";

        newOutput += (this->info != L"" ? L"<args>" + this->info + L"</args>" : L"");

        if (this->followUp.get() != NULL) {
            newOutput = this->followUp.get()->Serialize(newOutput);
        }

        return newOutput;
    }
}