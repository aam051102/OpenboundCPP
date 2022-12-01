#ifndef SBURB_ASSET_MUSIC_H
#define SBURB_ASSET_MUSIC_H

#include "Common.h"
#include <SFML/Audio/Music.hpp>

namespace SBURB
{
    class AssetMusic
    {
    public:
        AssetMusic(std::string path, float startLoop = 0);

        void SetLoopPoints(float start);
        void Loop();
        void Play(float pos);
        void Pause();
        void Stop();
        bool Ended();
        void FixVolume();

        void SetName(std::string name) { this->name = name; };
        std::string GetName() { return this->name; };

        std::string GetType() { return this->type; };

    private:
        float startLoop;
        
        std::string path;
        std::string name;
        std::string type;

        std::unique_ptr<sf::Music> asset;

    };
}

#endif