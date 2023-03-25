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
        AssetGraphic(std::wstring name, std::wstring path);

        std::shared_ptr<sf::Texture> GetAsset() { return this->asset; };

        std::wstring GetPath() { return this->path; };

    private:
        std::wstring path;
        std::shared_ptr<sf::Texture> asset;

    };
}

#endif