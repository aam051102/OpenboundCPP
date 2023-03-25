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
        Sound(std::wstring name, std::shared_ptr<AssetAudio> audio);

        void Play(float pos = 0);
        void Pause();
        void Stop();
        bool Ended();
        void FixVolume();

        void SetName(std::wstring name) { this->name = name; };
        std::wstring GetName() { return this->name; };

        std::wstring GetType() { return this->type; };

    private:
        std::wstring name;
        std::wstring type;

        sf::Sound asset;
        std::shared_ptr<AssetAudio> audio;
    };
}

#endif