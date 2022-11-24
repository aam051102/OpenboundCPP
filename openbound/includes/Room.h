#ifndef SBURB_ROOM_H
#define SBURB_ROOM_H

#include "Common.h"
#include "Zone.h"
#include "Animation.h"
#include "Path.h"
#include "Trigger.h"

namespace SBURB
{
    class Room
    {
    public:
        Room(std::string name, int width, int height);


    private:
		std::string name;
		int width;
		int height;
		std::vector<Sprite> sprites;
		std::vector<Animation> effects;
		std::vector<Path> walkables;
		std::vector<Path> unwalkables;
		std::vector<Path> motionPaths;
		std::vector<Trigger> triggers;
		std::string walkableMap;
		int mapScale;
    };
}

#endif
