#include "EventFactory.h"
#include "EventGameState.h"
#include "EventInBox.h"
#include "EventInBox2.h"
#include "EventMovie.h"
#include "EventNoActions.h"
#include "EventNudge.h"
#include "EventPlayed.h"
#include "EventSpriteProperty.h"
#include "EventTime.h"
#include "EventWithinRange.h"
#include <vector>

namespace SBURB {
	Event* EventFactory::CreateEvent(std::string type, std::string info) {
		// Parse params
		std::vector<std::string> params = split(info, ',');
		for (int i = 0; i < params.size(); i++) {
			params[i] = trim(params[i]);
		}

		// Create event
		if (type == "gameState") {
			return new EventGameState(params[0]);
		}
		
		if (type == "inBox") {
			return new EventInBox(params[0], stoi(params[1]), stoi(params[2]), stoi(params[3]), stoi(params[4]));
		}

		if (type == "inBox2") {
			return new EventInBox2(params[0], stoi(params[1]), stoi(params[2]), stoi(params[3]), stoi(params[4]));
		}

		if (type == "movie") {
			return new EventMovie(params[0]);
		}

		if (type == "noActions") {
			return new EventNoActions();
		}
		
		if (type == "nudge") {
			return new EventNudge();
		}

		if (type == "played") {
			return new EventPlayed(params[0]);
		}

		if (type == "spriteProperty") {
			return new EventSpriteProperty(params[0], params[1]);
		}

		if (type == "time") {
			return new EventTime();
		}

		if (type == "withinRange") {
			return new EventWithinRange(params[0], params[1], stoi(params[2]));
		}
	}
}