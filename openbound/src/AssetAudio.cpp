#include "AssetAudio.h"
#include "Sburb.h"
#include "AssetManager.h"

namespace SBURB {
    AssetAudio::AssetAudio(std::wstring name, std::vector<std::wstring> sources) {
        this->type = L"audio";
        this->name = name;
        this->sources = sources;
        this->asset = std::make_shared<sf::SoundBuffer>();
        const auto resolvedPath = Sburb::ResolvePath(sources[0]);
        this->asset->loadFromFile(std::string(resolvedPath.begin(), resolvedPath.end()));

        AssetManager::AddFilePath(name, Sburb::ResolvePath(sources[0]));
    }
}