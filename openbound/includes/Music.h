#ifndef SBURB_MUSIC_H
#define SBURB_MUSIC_H

#include "Common.h"
#include <SFML/Audio/Music.hpp>

namespace SBURB
{
    class Music
    {
    public:
        Music(std::wstring name, double startLoop = 0);

        void SetLoopPoints(double start);
        void Loop();
        void Play(double pos = 0);
        void Pause();
        void Stop();
        bool Ended();
        void FixVolume();

        void SetName(std::wstring name) { this->name = name; };
        std::wstring GetName() { return this->name; };

        std::wstring GetType() { return this->type; };

        std::shared_ptr<sf::Music> GetAsset() { return this->asset; };

        double GetStartLoop() { return this->startLoop; };

    private:
        double startLoop;
        
        std::wstring path;
        std::wstring name;
        std::wstring type;

        std::shared_ptr<sf::Music> asset;

    };
}

#endif