#ifndef SBURB_ASSET_TEXTURE_H
#define SBURB_ASSET_TEXTURE_H

#include "Common.h"
#include <SFML/Graphics/Texture.hpp>

namespace SBURB
{
    class AssetTexture : public sf::Texture
    {
    public:
        void SetName(std::string name) { this->name = name; };
        std::string GetName() { return this->GetName(); };

    private:
        std::string name;
    };
}

#endif