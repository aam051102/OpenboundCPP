#ifndef SBURB_ASSET_GRAPHIC_H
#define SBURB_ASSET_GRAPHIC_H

#include "Common.h"
#include <SFML/Graphics/Texture.hpp>
#include "Asset.h"

namespace SBURB
{
    class AssetGraphic : public Asset
    {
    public:
        AssetGraphic(std::string name, std::string path);

        std::shared_ptr<sf::Texture> Load();
        void Unload();

        std::string GetPath() { return this->path; };

    private:
        std::string path;
        std::shared_ptr<sf::Texture> asset;
        uint8_t count = 0;
    };
}

#endif