#include "AssetAudio.h"
#include "Sburb.h"
#include "AssetManager.h"

namespace SBURB {
    AssetAudio::AssetAudio(std::string name, std::vector<std::string> sources) {
        this->type = "audio";
        this->name = name;
        this->sources = sources;
        this->asset = std::make_shared<sf::SoundBuffer>();
        this->asset->loadFromFile(Sburb::ResolvePath(sources[0]));
        std::cout << name << ", " << Sburb::ResolvePath(sources[0]) << std::endl;
        AssetManager::AddFilePath(name, Sburb::ResolvePath(sources[0]));
    }
}