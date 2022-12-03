#ifndef SBURB_ASSET_GRAPHIC_H
#define SBURB_ASSET_GRAPHIC_H

#include "Common.h"
#include <SFML/Graphics/Texture.hpp>
#include "Asset.h"

namespace SBURB
{
    class AssetGraphic: public Asset
    {
    public:
        AssetGraphic(std::string name, std::string path) {
            this->type = "graphic";
            this->name = name;
            this->path = path;
            this->asset = std::make_shared<sf::Texture>();
            this->asset->loadFromFile(Sburb::ResolvePath(path));
        };

        std::shared_ptr<sf::Texture> GetAsset() { return this->asset; };

        std::string GetPath() { return this->path; };

    private:
        std::string path;
        std::shared_ptr<sf::Texture> asset;

    };
}

#endif