#ifndef SBURB_ASSET_AUDIO_H
#define SBURB_ASSET_AUDIO_H

#include "Common.h"
#include <SFML/Audio/SoundBuffer.hpp>
#include "Asset.h"

namespace SBURB
{
    class AssetAudio : public Asset
    {
    public:
        AssetAudio(std::string name, std::vector<std::string> sources) {
            this->type = "audio";
            this->name = name;
            this->sources = sources;
            this->asset = std::make_shared<sf::SoundBuffer>();
            this->asset->loadFromFile(Sburb::ResolvePath(sources[0]));
        };

        std::shared_ptr<sf::SoundBuffer> GetAsset() { return this->asset; };

        std::vector<std::string> GetSources() { return this->sources; };

    private:
        std::vector<std::string> sources;
        std::shared_ptr<sf::SoundBuffer> asset;

    };
}

#endif