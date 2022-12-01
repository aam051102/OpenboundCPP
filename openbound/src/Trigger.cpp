#include "Trigger.h"
#include "Sburb.h"
#include "EventFactory.h"

namespace SBURB {
    Trigger::Trigger(std::vector<std::string> info, std::shared_ptr<Action> action, std::shared_ptr<Trigger> followUp, bool shouldRestart, bool shouldDetonate, std::string op) {
        this->info = info;
        this->action = action;
        this->followUp = followUp;
        this->shouldRestart = shouldRestart;
        this->shouldDetonate = shouldDetonate;
        this->op = op;
        this->waitFor = NULL;

        this->events = {};
        for (int i = 0; i < info.size(); i++) {
            std::string inf = trim(this->info[i]);
            std::vector<std::string> params = split(inf, ",");
            std::string type = params[0];

            this->events.push_back(EventFactory::CreateEvent(type, inf));
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

    }

    bool Trigger::TryToTrigger() {
        if (this->waitFor) {
            if (this->waitFor->CheckCompletion()) {
                this->waitFor = NULL;
            }
            else {
                return false;
            }
        }

        if (this->CheckCompletion()) {
            if (this->action) {
                std::shared_ptr<ActionQueue> result = Sburb::GetInstance()->PerformAction(this->action);

                if (result) {
                    this->waitFor = std::make_shared<Trigger>("noActions," + result->GetId());
                }
                else {
                    this->waitFor = std::make_shared<Trigger>("noActions");
                }
            }

            if (this->followUp) {
                if (this->followUp->TryToTrigger()) {
                    this->followUp = NULL;
                }
            }

            if (this->shouldRestart) {
                this->Reset();
            }

            return this->shouldDetonate;
        }
    }

    std::string Trigger::Serialize(std::string output) {
        std::string newOutput = output + "\n<trigger" +
            (this->shouldRestart ? " restart='true'" : "") +
            (this->shouldDetonate ? " detonate='true'" : "") +
            (this->op != "" ? " operator='" + this->op + "'" : "") +
            ">";
        for (int i = 0; i < this->info.size(); i++) {
            if (this->events[i]->canSerialize) {
                newOutput = newOutput + "<args>" + escape(this->events[i]->Serialize().c_str()) + "</args>";
            }
            else {
                newOutput = newOutput + "<args>" + escape(this->info[i].c_str()) + "</args>";
            }
        }
        if (this->action) {
            newOutput = this->action->Serialize(newOutput);
        }
        if (this->followUp) {
            newOutput = this->followUp->Serialize(newOutput);
        }

        newOutput = newOutput + "\n</trigger>";

        return newOutput;
    }
}