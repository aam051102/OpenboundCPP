#ifndef SBURB_ASSET_TEXTURE_H
#define SBURB_ASSET_TEXTURE_H

#include "Common.h"
#include <SFML/Graphics/Texture.hpp>

namespace SBURB
{
    class AssetTexture : public sf::Texture
    {
    public:
        AssetTexture() {
            this->type = "graphic";
        };

        void SetName(std::string name) { this->name = name; };
        std::string GetName() { return this->name; };

        std::string GetType() { return this->type; };

    private:
        std::string name;
        std::string type;

    };
}

#endif