#include "AssetMusic.h"
#include "Sburb.h"

namespace SBURB {
    AssetMusic::AssetMusic(std::string path, float startLoop) {
        this->type = "music";
        this->path = path;
        this->startLoop = startLoop;

        this->asset = std::make_shared<sf::Music>();
        this->asset->openFromFile(path);
       
        this->SetLoopPoints(startLoop);
    }

    void AssetMusic::SetLoopPoints(float start)
    {
        this->startLoop = start;
        this->asset->setLoopPoints(sf::Music::TimeSpan(sf::seconds(start), this->asset->getDuration()));
    }

    void AssetMusic::Loop()
    {
        this->asset->setLoop(true);
        this->asset->setPlayingOffset(sf::seconds(this->startLoop));
        this->asset->play();
    }

    void AssetMusic::Play(float pos)
    {
        this->asset->setLoop(false);
        this->asset->setPlayingOffset(sf::seconds(pos));
        this->asset->play();
    }

    void AssetMusic::Pause()
    {
        this->asset->pause();
    }

    void AssetMusic::Stop()
    {
        this->asset->stop();
    }

    bool AssetMusic::Ended()
    {
        return this->asset->getPlayingOffset() == this->asset->getDuration() && this->asset->getStatus() != sf::SoundStream::Playing;
    }

    void AssetMusic::FixVolume()
    {
        this->asset->setVolume(Sburb::GetInstance()->GetGlobalVolume());
    }
}