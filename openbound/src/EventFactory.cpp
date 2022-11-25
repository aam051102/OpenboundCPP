#include "EventFactory.h"
#include "EventGameState.h"
#include "EventInBox.h"
#include "EventMovie.h"
#include "EventNoActions.h"
#include "EventNudge.h"
#include "EventPlayed.h"
#include "EventSpriteProperty.h"
#include "EventTime.h"
#include "EventWithinRange.h"
#include <vector>

namespace SBURB {
	std::shared_ptr<Event> EventFactory::CreateEvent(std::string type, std::string info) {
		// Parse params
		std::vector<std::string> params = split(info, ",");
		for (int i = 0; i < params.size(); i++) {
			params[i] = trim(params[i]);
		}

		/// Create event
		if (type == "gameState") {
			return std::make_shared<EventGameState>(params[0]);
		}
		
		if (type == "inBox") {
			return std::make_shared<EventInBox>(params[0], stoi(params[1]), stoi(params[2]), stoi(params[3]), stoi(params[4]));
		}

		if (type == "inBox2") {
			// Alternate method of creating inBox event
			return std::make_shared<EventInBox>(
				params[0],
				std::min(stoi(params[1]), stoi(params[3])),
				std::min(stoi(params[2]), stoi(params[4])),
				abs(stoi(params[1]) - stoi(params[3])),
				abs(stoi(params[2]) - stoi(params[4]))
				);
		}

		if (type == "movie") {
			return std::make_shared<EventMovie>(params[0]);
		}

		if (type == "noActions") {
			return std::make_shared<EventNoActions>();
		}
		
		if (type == "nudge") {
			return std::make_shared<EventNudge>();
		}

		if (type == "played") {
			return std::make_shared<EventPlayed>(params[0]);
		}

		if (type == "spriteProperty") {
			return std::make_shared<EventSpriteProperty>(params[0], params[1]);
		}

		if (type == "time") {
			return std::make_shared<EventTime>();
		}

		if (type == "withinRange") {
			return std::make_shared<EventWithinRange>(params[0], params[1], stoi(params[2]));
		}
	}
}