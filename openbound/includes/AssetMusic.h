#ifndef SBURB_ASSET_MUSIC_H
#define SBURB_ASSET_MUSIC_H

#include "Common.h"
#include <SFML/Audio/Music.hpp>

namespace SBURB
{
    class AssetMusic
    {
    public:
        AssetMusic() {
            this->type = "music";
        };

        void SetName(std::string name) { this->name = name; };
        std::string GetName() { return this->name; };

        std::string GetType() { return this->type; };

    private:
        std::string name;
        std::string type;
        sf::Music asset;

    };
}

#endif