#ifndef SBURB_SOUND_H
#define SBURB_SOUND_H

#include "Common.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include "AssetAudio.h"

namespace SBURB
{
    class Sound
    {
    public:
        Sound(std::string name, std::shared_ptr<AssetAudio> audio);

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
        std::string type;

        sf::Sound asset;
        std::shared_ptr<AssetAudio> audio;
    };
}

#endif