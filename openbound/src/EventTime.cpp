#include "EventTime.h"

namespace SBURB {
    EventTime::EventTime(int time) {
        this->canSerialize = true;
        this->originalTime = time;
        this->time = 0;
    }

    EventTime::~EventTime() {

    }

    void EventTime::Reset() {
        this->time = this->originalTime;
    }

    bool EventTime::CheckCompletion() {
        this->time--;
        return this->time <= 0;
    }

    std::wstring EventTime::Serialize() {
        return L"time," + this->time;
    }
}