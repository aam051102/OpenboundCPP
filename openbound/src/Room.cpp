#include "Room.h"

constexpr int BLOCK_SIZE = 500;

namespace SBURB
{
    Room::Room(std::string name, int width, int height) {
		this->name = name;
		this->width = width;
		this->height = height;
		this->sprites = {};
		this->effects = {};
		this->walkables = {};
		this->unwalkables = {};
		this->motionPaths = {};
		this->triggers = {};
		this->walkableMap = "";
		this->mapScale = 4;
    }
}
