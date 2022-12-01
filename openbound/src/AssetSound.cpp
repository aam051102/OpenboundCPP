#include "AssetSound.h"
#include "Sburb.h"

namespace SBURB {
    AssetSound::AssetSound(std::string path) {
        this->type = "sound";
        this->path = path;

        sf::SoundBuffer buffer;
        if (buffer.loadFromFile("sound.wav")) {
            this->buffer = buffer;
            this->asset = sf::Sound();
            this->asset.setBuffer(this->buffer);
        }        
    }

    void AssetSound::Play(float pos)
    {
        this->asset.setPlayingOffset(sf::seconds(pos));

        this->FixVolume();
        this->asset.play();
    }

    void AssetSound::Pause()
    {
        this->asset.pause();
    }

    void AssetSound::Stop()
    {
        this->asset.stop();
    }

    bool AssetSound::Ended()
    {
        return this->asset.getPlayingOffset() >= this->buffer.getDuration();
    }
    
    void AssetSound::FixVolume()
    {
        this->asset.setVolume(Sburb::GetInstance()->GetGlobalVolume());
    }
}