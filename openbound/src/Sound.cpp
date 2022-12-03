#include "Sound.h"
#include "Sburb.h"

namespace SBURB
{
    Sound::Sound(std::string name, std::shared_ptr<sf::SoundBuffer> buffer)
    {
        this->name = name;
        this->type = "sound";
        this->buffer = buffer;
        this->asset = sf::Sound();
        this->asset.setBuffer(*this->buffer);
    }

    void Sound::Play(float pos)
    {
        this->asset.setPlayingOffset(sf::seconds(pos));

        this->FixVolume();
        this->asset.play();
    }

    void Sound::Pause()
    {
        this->asset.pause();
    }

    void Sound::Stop()
    {
        this->asset.stop();
    }

    bool Sound::Ended()
    {
        return this->asset.getPlayingOffset() >= this->buffer->getDuration();
    }

    void Sound::FixVolume()
    {
        this->asset.setVolume(Sburb::GetInstance()->GetGlobalVolume());
    }
}