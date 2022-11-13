#include "EventGameState.h"

// NOTE: This can obviously be optimized in the future.
namespace SBURB {
    EventGameState::EventGameState(std::string query) {
        this->canSerialize = false;

        this->token = "";
        if (query.find(">") != -1) this->token = ">";
        else if (query.find("GREATER") != -1) this->token = "GREATER";
        else if (query.find("<") != -1) this->token = "<";
        else if (query.find("LESS") != -1) this->token = "LESS";
        else if (query.find("!=") != -1) this->token = "!=";
        else if (query.find("=") != -1) this->token = "=";

        auto queryParts = split(query, this->token);

        this->prop = trim(queryParts[0]);
        this->target = trim(queryParts[1]);
    }

    EventGameState::~EventGameState() {

    }

    void EventGameState::Reset() {
    }

    bool EventGameState::CheckCompletion() {
        if (token == ">" || token == "GREATER") {
            return std::stoi(Sburb.gameState[prop]) > std::stoi(target);
        }

        if (token == "<" || token == "LESS") {
            return std::stoi(Sburb.gameState[prop]) < std::stoi(target);
        }

        if (token == "!=") {
            return Sburb.gameState[prop] != target;
        }

        if (token == "=") {
            return Sburb.gameState[prop] == target;
        }
    }
}