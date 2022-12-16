#ifndef SBURB_MUSIC_H
#define SBURB_MUSIC_H

#include "Common.h"
#include <SFML/Audio/Music.hpp>

namespace SBURB
{
    class Music
    {
    public:
        Music(std::string name, double startLoop = 0);

        void SetLoopPoints(double start);
        void Loop();
        void Play(double pos = 0);
        void Pause();
        void Stop();
        bool Ended();
        void FixVolume();

        void SetName(std::string name) { this->name = name; };
        std::string GetName() { return this->name; };

        std::string GetType() { return this->type; };

        std::shared_ptr<sf::Music> GetAsset() { return this->asset; };

        double GetStartLoop() { return this->startLoop; };

    private:
        double startLoop;
        
        std::string path;
        std::string name;
        std::string type;

        std::shared_ptr<sf::Music> asset;

    };
}

#endif