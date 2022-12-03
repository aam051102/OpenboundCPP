#include "AssetGraphic.h"
#include "Sburb.h"

namespace SBURB {
    AssetGraphic::AssetGraphic(std::string name, std::string path) {
        this->type = "graphic";
        this->name = name;
        this->path = path;
        this->asset = std::make_shared<sf::Texture>();
        this->asset->loadFromFile(Sburb::ResolvePath(path));
    }
}