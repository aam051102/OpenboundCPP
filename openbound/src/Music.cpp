#include "Music.h"
#include "Sburb.h"
#include "AssetManager.h"

namespace SBURB {
    Music::Music(std::string name, float startLoop) {
        this->type = "music";
        this->path = path;
        this->startLoop = startLoop;

        this->asset = std::make_shared<sf::Music>();
        this->asset->openFromFile(AssetManager::GetFilePathByName(name));
       
        this->SetLoopPoints(startLoop);
    }

    void Music::SetLoopPoints(float start)
    {
        this->startLoop = start;
        this->asset->setLoopPoints(sf::Music::TimeSpan(sf::seconds(start), this->asset->getDuration()));
    }

    void Music::Loop()
    {
        this->asset->setLoop(true);
        this->asset->setPlayingOffset(sf::seconds(this->startLoop));
        this->asset->play();
    }

    void Music::Play(float pos)
    {
        this->asset->setLoop(false);
        this->asset->setPlayingOffset(sf::seconds(pos));
        this->asset->play();
    }

    void Music::Pause()
    {
        this->asset->pause();
    }

    void Music::Stop()
    {
        this->asset->stop();
    }

    bool Music::Ended()
    {
        return this->asset->getPlayingOffset() == this->asset->getDuration() && this->asset->getStatus() != sf::SoundStream::Playing;
    }

    void Music::FixVolume()
    {
        this->asset->setVolume(Sburb::GetInstance()->GetGlobalVolume());
    }
}