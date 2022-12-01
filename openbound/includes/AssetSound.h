#ifndef SBURB_ASSET_SOUND_H
#define SBURB_ASSET_SOUND_H

#include "Common.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace SBURB
{
    class AssetSound
    {
    public:
        AssetSound(std::string path);

        void Play(float pos = 0);
        void Pause();
        void Stop();
        bool Ended();
        void FixVolume();

        void SetName(std::string name) { this->name = name; };
        std::string GetName() { return this->name; };

        std::string GetType() { return this->type; };

    private:
        std::string name;
        std::string path;
        std::string type;

        sf::Sound asset;
        sf::SoundBuffer buffer;

    };
}

#endif