#include "EventMovie.h"

namespace SBURB {
    EventMovie::EventMovie(std::string movieName, int threshold) {
        this->movieName = movieName;
        this->threshold = threshold;
    }

    EventMovie::~EventMovie() {

    }

    void EventMovie::Reset() {

    }

    bool EventMovie::CheckCompletion() {
        // MOVIES NOT SUPPORTED.
        return true;
    }
}