#include "EventGameState.h"
#include "Sburb.h"

// NOTE: This can obviously be optimized in the future.
namespace SBURB {
    EventGameState::EventGameState(std::wstring query) {
        this->canSerialize = false;

        this->token = L"";
        if (query.find(L">") != std::wstring::npos) this->token = L">";
        else if (query.find(L"GREATER") != std::wstring::npos) this->token = L"GREATER";
        else if (query.find(L"<") != std::wstring::npos) this->token = L"<";
        else if (query.find(L"LESS") != std::wstring::npos) this->token = L"LESS";
        else if (query.find(L"!=") != std::wstring::npos) this->token = L"!=";
        else if (query.find(L"=") != std::wstring::npos) this->token = L"=";

        auto queryParts = split(query, this->token);

        this->prop = trim(queryParts[0]);
        this->target = trim(queryParts[1]);
    }

    EventGameState::~EventGameState() {

    }

    void EventGameState::Reset() {
    }

    bool EventGameState::CheckCompletion() {
        if (token == L">" || token == L"GREATER") {
            return std::stoi(Sburb::GetInstance()->GetGameState(this->prop)) > std::stoi(target);
        }

        if (token == L"<" || token == L"LESS") {
            return std::stoi(Sburb::GetInstance()->GetGameState(this->prop)) < std::stoi(target);
        }

        if (token == L"!=") {
            return Sburb::GetInstance()->GetGameState(this->prop) != target;
        }

        if (token == L"=") {
            return Sburb::GetInstance()->GetGameState(this->prop) == target;
        }

        return false;
    }
}