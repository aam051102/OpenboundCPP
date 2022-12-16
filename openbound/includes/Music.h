#ifndef SBURB_MUSIC_H
#define SBURB_MUSIC_H

#include "Common.h"
#include <SFML/Audio/Music.hpp>

namespace SBURB
{
    class Music
    {
    public:
        Music(std::string name, float startLoop = 0);

        void SetLoopPoints(float start);
        void Loop();
        void Play(float pos = 0);
        void Pause();
        void Stop();
        bool Ended();
        void FixVolume();

        void SetName(std::string name) { this->name = name; };
        std::string GetName() { return this->name; };

        std::string GetType() { return this->type; };

        std::shared_ptr<sf::Music> GetAsset() { return this->asset; };

        float GetStartLoop() { return this->startLoop; };

    private:
        float startLoop;
        
        std::string path;
        std::string name;
        std::string type;

        std::shared_ptr<sf::Music> asset;

    };
}

#endif