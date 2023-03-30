#include "Music.h"
#include "Sburb.h"
#include "AssetManager.h"

namespace SBURB
{
    Music::Music(std::wstring name, double startLoop)
    {
        this->type = L"music";
        this->path = path;
        this->startLoop = startLoop;
        this->name = name;

        this->asset = std::make_shared<sf::Music>();
        const auto resolvedPath = AssetManager::GetFilePathByName(name);
        this->asset->openFromFile(std::string(resolvedPath.begin(), resolvedPath.end()));

        this->SetLoopPoints(startLoop);
    }

    void Music::SetLoopPoints(double start)
    {
        this->startLoop = start;

        sf::Music::TimeSpan span = sf::Music::TimeSpan();
        span.offset = sf::milliseconds(start);
        span.length = this->asset->getDuration() - sf::milliseconds(start);
        this->asset->setLoopPoints(span);

        this->asset->setLoop(true);
    }

    void Music::Loop()
    {
        this->Play(this->startLoop);
    }

    void Music::Play(double pos)
    {
        this->FixVolume();
        this->asset->setPlayingOffset(sf::milliseconds(pos));
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