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
        AssetAudio(std::wstring name, std::vector<std::wstring> sources);

        std::shared_ptr<sf::SoundBuffer> GetAsset() { return this->asset; };

        std::vector<std::wstring> GetSources() { return this->sources; };

    private:
        std::vector<std::wstring> sources;
        std::shared_ptr<sf::SoundBuffer> asset;

    };
}

#endif