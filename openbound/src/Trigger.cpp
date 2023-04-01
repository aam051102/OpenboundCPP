#include "Trigger.h"
#include "Sburb.h"
#include "EventFactory.h"

namespace SBURB {
    Trigger::Trigger(std::vector<std::wstring> info, std::shared_ptr<Action> action, std::shared_ptr<Trigger> followUp, bool shouldRestart, bool shouldDetonate, std::wstring op) {
        this->info = info;
        this->action = action;
        this->followUp = followUp;
        this->shouldRestart = shouldRestart;
        this->shouldDetonate = shouldDetonate;

        if (op == L"") op = L"AND";
        this->op = op;

        this->waitFor = NULL;

        this->events = {};
        for (int i = 0; i < info.size(); i++) {
            std::wstring inf = trim(this->info[i]);

            this->events.push_back(EventFactory::CreateEvent(inf));
        }

        this->Reset();
    }

    Trigger::~Trigger() {

    }

    void Trigger::Reset() {
        for (int i = 0; i < this->events.size(); i++) {
            this->events[i]->Reset();
        }
    }

    bool Trigger::CheckCompletion() {
        if (this->op == L"AND") {
            bool result = true;

            for (int i = 0; i < this->events.size(); i++) {
                result = result && this->events[i]->CheckCompletion();
            }

            return result;
        }
        else if (this->op == L"NAND") {
            bool result = true;

            for (int i = 0; i < this->events.size(); i++) {
                result = result && this->events[i]->CheckCompletion();
            }

            return !result;
        }
        else if (this->op == L"NOT") {
            bool result = true;

            for (int i = 0; i < this->events.size(); i++) {
                result = result && this->events[i]->CheckCompletion();
            }

            return !result;
        }
        else if (this->op == L"OR") {
            bool result = false;

            for (int i = 0; i < this->events.size(); i++) {
                result = result || this->events[i]->CheckCompletion();
            }

            return result;
        }
        else if (this->op == L"NOR") {
            bool result = false;

            for (int i = 0; i < this->events.size(); i++) {
                result = result || this->events[i]->CheckCompletion();
            }

            return !result;
        }
        else if (this->op == L"XOR") {
            bool result = false;

            for (int i = 0; i < this->events.size(); i++) {
                if (this->events[i]->CheckCompletion()) {
                    if (result) {
                        return false; //*EXCLUSIVE* OR!
                    }
                    else {
                        result = true;
                    }
                }
            }

            return result;
        }

        return false;
    }

    bool Trigger::TryToTrigger() {
        if (this->waitFor) {
            if (this->waitFor->CheckCompletion()) {
                this->waitFor = nullptr;
            }
            else {
                return false;
            }
        }

        if (this->CheckCompletion()) {
            if (this->action) {
                std::shared_ptr<ActionQueue> result = Sburb::GetInstance()->PerformAction(this->action);

                if (result) {
                    this->waitFor = std::make_shared<Trigger>(std::vector<std::wstring>({ L"noActions," + result->GetId() }));
                }
                else {
                    this->waitFor = std::make_shared<Trigger>(std::vector<std::wstring>({ L"noActions" }));
                }
            }

            if (this->followUp) {
                if (this->followUp->TryToTrigger()) {
                    this->followUp = nullptr;
                }
            }

            if (this->shouldRestart) {
                this->Reset();
            }

            return this->shouldDetonate;
        }

        return false;
    }

    std::wstring Trigger::Serialize(std::wstring output) {
        std::wstring newOutput = output + L"\n<trigger" +
            (this->shouldRestart ? L" restart='true'" : L"") +
            (this->shouldDetonate ? L" detonate='true'" : L"") +
            (this->op != L"" ? L" operator='" + this->op + L"'" : L"") +
            L">";
        for (int i = 0; i < this->info.size(); i++) {
            if (this->events[i]->canSerialize) {
                newOutput = newOutput + L"<args>" + escape(this->events[i]->Serialize().c_str()) + L"</args>";
            }
            else {
                newOutput = newOutput + L"<args>" + escape(this->info[i].c_str()) + L"</args>";
            }
        }
        if (this->action) {
            newOutput = this->action->Serialize(newOutput);
        }
        if (this->followUp) {
            newOutput = this->followUp->Serialize(newOutput);
        }

        newOutput = newOutput + L"\n</trigger>";

        return newOutput;
    }
}