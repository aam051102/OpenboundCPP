#ifndef SBURB_ASSET_SOUND_H
#define SBURB_ASSET_SOUND_H

#include "Common.h"
#include <SFML/Audio/Sound.hpp>

namespace SBURB
{
    class AssetSound
    {
    public:
        AssetSound() {
            this->type = "sound";
        };

        void SetName(std::string name) { this->name = name; };
        std::string GetName() { return this->name; };

        std::string GetType() { return this->type; };

    private:
        std::string name;
        std::string type;
        sf::Sound asset;

    };
}

#endif